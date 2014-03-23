#include "collision_bitmap.hpp"
#include "collision_helpers.hpp"

#include "resources/pattern.hpp"
#include "resources/terrain_library.hpp"

#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>

namespace
{
    static const std::size_t num_rotations = 64;
}

namespace ts
{
    namespace world
    {
        Vector2u calculate_bitmap_size(const resources::Pattern& pattern);

        std::vector<std::uint64_t> generate_rotated_bitmaps(const resources::Pattern& pattern, Vector2i bitmap_size);

        std::size_t rotation_frame(Rotation<double> rotation);

        Collision_point collision_test_impl(const std::uint64_t* subject_data, const std::uint64_t* object_data,
                                            Vector2i subject_size, Vector2i object_size,
                                            Vector2i subject_position, Vector2i object_position,
                                            Vector2i subject_center, Vector2i object_center);
    }
}

std::size_t ts::world::rotation_frame(Rotation<double> rotation)
{
    const double frame_factor = num_rotations / 360.0;
    auto frame_id = static_cast<int>(std::floor(rotation.degrees() * frame_factor + 0.5)) + num_rotations;
    if (frame_id >= num_rotations) frame_id -= num_rotations;

    return frame_id;
}

ts::Vector2u ts::world::calculate_bitmap_size(const resources::Pattern& pattern)
{
    Vector2u size = pattern.size();

    auto max_size = std::max(size.x, size.y);
    Vector2u result{ max_size, max_size };

    result.x <<= 1;
    if ((result.x & 63) != 0) {
        // Make the width a multiple of 32
        result.x |= 63;
        ++result.x;
    }

    result.y = result.x;
    return result;
}

std::vector<std::uint64_t> ts::world::generate_rotated_bitmaps(const resources::Pattern& pattern, Vector2i bitmap_size)
{
    std::vector<std::uint64_t> bitmap(bitmap_size.y * (bitmap_size.x >> 6) * num_rotations);

    Vector2i pattern_size = pattern.size();

    Vector2i dest_center(bitmap_size.x >> 1, bitmap_size.y >> 1);
    Vector2i source_center(pattern_size.x >> 1, pattern_size.y >> 1);

    Rotation<double> rotation, increment = Rotation<double>::degrees(360.0 / num_rotations);

    auto row_width = bitmap_size.x >> 6;

    for (int r = 0; r != num_rotations; ++r, rotation += increment)
    {
        /*
        auto sin = std::sin(rotation.radians());
        auto cos = std::cos(rotation.radians());

        Vector2<double> dest_point(-dest_center.x, -dest_center.y);

        auto entry_ptr = &bitmap[bitmap_size.y * row_width * r];
        
        for (std::size_t y = 0; y != bitmap_size.y; ++y, dest_point.y += 1.0)
        {
            dest_point.x = -dest_center.x;

            for (std::size_t x = 0; x != bitmap_size.x; )
            {
                std::uint64_t entry = 0;

                for (std::size_t x_end = x + 64; x != x_end; ++x, dest_point.x += 1.0)
                {
                    auto transformed = transform_point(dest_point, -sin, cos);
                    transformed.x = std::floor(transformed.x + 0.5);
                    transformed.y = std::floor(transformed.y + 0.5);

                    auto source_point = transformed;
                    source_point += source_center;

                    entry <<= 1;
                    
                    if (source_point.x >= 0 && source_point.y >= 0 &&
                        source_point.x < pattern_size.x && source_point.y < pattern_size.y &&
                        pattern(source_point.x, source_point.y))
                    {
                        entry |= 1;
                    }
                }

                *entry_ptr++ = entry;
            }
        }
        */
        
        auto sin = std::sin(rotation.radians());
        auto cos = std::cos(rotation.radians());

        Vector2<double> source_point(-source_center.x, -source_center.y);

        auto base_ptr = &bitmap[bitmap_size.y * row_width * r];
        
        for (std::size_t y = 0; y != pattern_size.y; ++y, source_point.y += 1.0)
        {
            source_point.x = -source_center.x;

            for (std::size_t x = 0; x != pattern_size.x; ++x, source_point.x += 1.0)
            {
                if (pattern(x, y) == 0) continue;

                auto transformed = transform_point(source_point, sin, cos);
                Vector2i dest_point(std::floor(transformed.x + 0.5), std::floor(transformed.y + 0.5));

                dest_point += dest_center;
            
                auto& entry = base_ptr[row_width * dest_point.y + (dest_point.x >> 6)];
                entry |= std::uint64_t(1) << (63 - (dest_point.x & 63));
            }
        }
    }

    return bitmap;
}


ts::world::Collision_bitmap::Collision_bitmap(const std::shared_ptr<resources::Pattern>& pattern)
  : bitmap_size_(calculate_bitmap_size(*pattern)),
    bitmap_(generate_rotated_bitmaps(*pattern, bitmap_size_))
{
}

const ts::Vector2u& ts::world::Collision_bitmap::size() const
{
    return bitmap_size_;
}

const std::vector<std::uint64_t>& ts::world::Collision_bitmap::bitmap() const
{
    return bitmap_;
}

bool ts::world::Collision_bitmap::operator()(Vector2i point) const
{
    point.x += bitmap_size_.x >> 1;
    point.y += bitmap_size_.y >> 1;

    if (point.x < 0 || point.y < 0 || point.x >= int(bitmap_size_.x) || point.y >= int(bitmap_size_.y))
        return false;

    auto row_width = bitmap_size_.x >> 6;
    auto entry = bitmap_[point.y * row_width + (point.x >> 6)];

    return (entry << (point.x & 63) >> 63) != 0;
}

bool ts::world::Collision_bitmap::operator()(Vector2i point, Rotation<double> rotation) const
{
    point.x += bitmap_size_.x >> 1;
    point.y += bitmap_size_.y >> 1;

    if (point.x < 0 || point.y < 0 || point.x >= int(bitmap_size_.x) || point.y >= int(bitmap_size_.y))
        return false;

    auto frame_id = rotation_frame(rotation);

    auto row_width = bitmap_size_.x >> 6;
    auto base_ptr = &bitmap_[bitmap_size_.y * row_width * frame_id];
    auto entry = base_ptr[point.y * row_width + (point.x >> 6)];

    return (entry << (point.x & 63) >> 63) != 0;
}

ts::Vector2i ts::world::Collision_bitmap::base_point(Vector2i point, Rotation<double> rotation) const
{
    const double frame_factor = num_rotations / 360.0;

    auto id = rotation_frame(rotation);
    auto frame_rotation = Rotation<double>::degrees(frame_factor * id);

    auto sin = std::sin(frame_rotation.radians());
    auto cos = std::cos(frame_rotation.radians());

    auto transformed = transform_point(point, -sin, cos);
    transformed.x = std::floor(transformed.x + 0.5);
    transformed.y = std::floor(transformed.y + 0.5);
    return transformed;
}

const std::shared_ptr<ts::world::Collision_bitmap>& ts::world::Collision_bitmap_store::operator[]
    (const std::shared_ptr<resources::Pattern>& pattern)
{
    auto it = bitmap_lookup_.find(pattern);
    if (it == bitmap_lookup_.end()) 
    {
        it = bitmap_lookup_.insert(std::make_pair(pattern, std::make_shared<Collision_bitmap>(pattern))).first;
    }

    return it->second;
}

ts::world::Static_collision_bitmap::Static_collision_bitmap(const resources::Pattern& pattern, 
                                                            const resources::Terrain_library& terrain_lib, std::size_t num_levels)
  : num_levels_(num_levels),
    bitmap_size_((((pattern.size().x - 1) >> 6) + 1) << 6, pattern.size().y)
{
    auto pattern_size = pattern.size();
    auto row_width = bitmap_size_.x >> 6;

    bitmap_.resize(row_width * bitmap_size_.y * std::max<std::size_t>(num_levels, 1));

    auto is_wall = [&](std::uint32_t x, std::uint32_t y, std::size_t level)
    {
        if (x >= pattern_size.x || y >= pattern_size.y) return false;

        auto terrain_id = pattern(x, y);
        terrain_id = terrain_lib.sub_terrain(terrain_id, level);
        return terrain_lib.terrain_by_id(terrain_id).is_wall;
    };

    std::uint64_t* bitmap_ptr = &bitmap_[0];

    // Quadruply nested for-loop. Good god that's awful
    for (std::size_t level = 0; level != num_levels; ++level) {
        for (std::uint32_t y = 0; y != bitmap_size_.y; ++y) {
            for (std::uint32_t x = 0; x < bitmap_size_.x;) {
                std::uint64_t entry = 0;

                for (std::uint32_t x_end = x + 64; x != x_end; ++x) {
                    entry <<= 1;
                    if (is_wall(x, y, level)) entry |= 1;
                }

                *bitmap_ptr++ = entry;
            }
        }
    }
}

const ts::Vector2u& ts::world::Static_collision_bitmap::size() const
{
    return bitmap_size_;
}


const std::vector<std::uint64_t>& ts::world::Static_collision_bitmap::bitmap() const
{
    return bitmap_;
}

bool ts::world::Static_collision_bitmap::operator()(const Vector2u& point, std::size_t level) const
{
    if (point.x >= bitmap_size_.x || point.y >= bitmap_size_.y) return false;

    auto entry = bitmap_[point.y * (bitmap_size_.x >> 6) + (point.x >> 6)];

    return (entry << (point.x & 63) >> 63) != 0;
}

ts::world::Collision_point ts::world::collision_test_impl(const std::uint64_t* subject_data, const std::uint64_t* object_data,
                                                          Vector2i subject_size, Vector2i object_size,
                                                          Vector2i subject_position, Vector2i object_position,
                                                          Vector2i subject_center, Vector2i object_center)
{
    Rect<int> subject_area(subject_position.x - subject_center.x, subject_position.y - subject_center.y,
                           subject_size.x, subject_size.y);

    Rect<int> object_area(object_position.x - object_center.x, object_position.y - object_center.y,
                           object_size.x, object_size.y);

    auto intersect_area = intersection(subject_area, object_area);
    if (intersect_area.left < 0)
    {
        intersect_area.width += intersect_area.left;
        intersect_area.left = 0;
    }

    if (intersect_area.top < 0)
    {
        intersect_area.height += intersect_area.top;
        intersect_area.top = 0;
    }

    if (intersect_area.width <= 0 || intersect_area.height <= 0) return{};

    Rect<int> subject_local_area(intersect_area.left - subject_position.x + subject_center.x,
                                 intersect_area.top - subject_position.y + subject_center.y,
                                 intersect_area.width, intersect_area.height);

    Rect<int> object_local_area(intersect_area.left - object_position.x + object_center.x,
                                intersect_area.top - object_position.y + object_center.y,
                                intersect_area.width, intersect_area.height);

    auto subject_offset = static_cast<unsigned>(subject_area.left) & 63;
    auto object_offset = static_cast<unsigned>(object_area.left) & 63;
    auto intersect_offset = static_cast<unsigned>(intersect_area.left) & 63;

    auto subject_inverse_offset = (64 - subject_offset) & 63;
    auto object_inverse_offset = (64 - object_offset) & 63;    

    auto subject_row_width = subject_size.x >> 6;
    auto object_row_width = object_size.x >> 6;

    auto x_begin = intersect_area.left >> 6 << 6;
    auto x_end = intersect_area.right();
    if ((x_end & 63) != 0) x_end = (x_end | 63) + 1;

    auto subject_row_begin = subject_data + (subject_row_width * subject_local_area.top);
    auto object_row_begin = object_data + (object_row_width * object_local_area.top);

    auto subject_begin = subject_row_begin;
    if (x_begin >= subject_area.left) subject_begin += (x_begin - subject_area.left) >> 6;
    else --subject_begin;

    auto object_begin = object_row_begin;
    if (x_begin >= object_area.left) object_begin += (x_begin - object_area.left) >> 6;
    else --object_begin;

    for (auto y = intersect_area.top; y != intersect_area.bottom(); ++y)
    {
        auto subject_ptr = subject_begin;
        auto object_ptr = object_begin;

        auto subject_row_end = subject_row_begin + subject_row_width;
        auto object_row_end = object_row_begin + object_row_width;

        for (auto x = x_begin; x != x_end; x += 64)
        {
            std::uint64_t subject_mask = 0, object_mask = 0;

            if (subject_offset)
            {
                if (subject_ptr >= subject_row_begin)
                    subject_mask |= *subject_ptr << subject_inverse_offset;

                if (++subject_ptr < subject_row_end)
                    subject_mask |= *subject_ptr >> subject_offset;
            }

            else {
                subject_mask |= *subject_ptr++;
            }

            if (object_offset) {
                if (object_ptr >= object_row_begin)
                    object_mask |= *object_ptr << object_inverse_offset;

                if (++object_ptr < object_row_end)
                    object_mask |= *object_ptr >> object_offset;
            }

            else {
                object_mask |= *object_ptr++;
            }

            if (auto mask = subject_mask & object_mask)
            {
                Collision_point result;
                result.collided = true;
                result.point.x = x; result.point.y = y;

                while ((mask & (std::uint64_t(1) << 63)) == 0)
                {
                    mask <<= 1;
                    ++result.point.x;
                }
                
                return result;
            }
        }

        subject_begin += subject_row_width;
        object_begin += object_row_width;

        subject_row_begin = subject_row_end;
        object_row_begin = object_row_end;
    }
        
    return {};
}

ts::world::Collision_point ts::world::collision_test(const Collision_bitmap& subject, const Collision_bitmap& object,
                                                     Vector2i subject_position, Vector2i object_position,
                                                     Rotation<double> subject_rotation, Rotation<double> object_rotation)
{    
    const auto& subject_bitmap = subject.bitmap();
    const auto& object_bitmap = object.bitmap();

    Vector2i subject_size = subject.size();
    Vector2i object_size = object.size();
   
    auto subject_data_size = subject_size.y * (subject_size.x >> 6);
    auto object_data_size = object_size.y * (object_size.x >> 6);

    Vector2i subject_center(subject_size.x >> 1, subject_size.y >> 1);
    Vector2i object_center(object_size.x >> 1, object_size.y >> 1);

    auto subject_frame_id = rotation_frame(subject_rotation);
    auto object_frame_id = rotation_frame(object_rotation);

    auto subject_data = &subject_bitmap[subject_frame_id * subject_data_size];
    auto object_data = &object_bitmap[object_frame_id * object_data_size];

    return collision_test_impl(subject_data, object_data, subject_size, object_size, 
                               subject_position, object_position, subject_center, object_center);
}

ts::world::Collision_point ts::world::collision_test(const Collision_bitmap& subject, const Static_collision_bitmap& scenery,
                                                     Vector2i subject_position, Rotation<double> subject_rotation, std::size_t subject_level)
{
    const auto& bitmap = subject.bitmap();
    const auto& scenery_bitmap = scenery.bitmap();
    Vector2i subject_size = subject.size();
    Vector2i scenery_size = scenery.size();

    Vector2i subject_center(subject_size.x >> 1, subject_size.y >> 1);

    Vector2i scenery_center(0, 0);
    Vector2i scenery_position(0, 0);

    auto scenery_data_size = scenery_size.y * (scenery_size.x >> 6);
    auto subject_data_size = subject_size.y * (subject_size.x >> 6);

    auto frame_id = rotation_frame(subject_rotation);

    auto subject_data = &bitmap[frame_id * subject_data_size];
    auto scenery_data = &scenery_bitmap[subject_level * scenery_data_size];

    return collision_test_impl(subject_data, scenery_data, subject_size, scenery_size, 
                               subject_position, scenery_position, subject_center, scenery_center);
    
}
