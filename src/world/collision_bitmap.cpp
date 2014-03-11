#include "collision_bitmap.hpp"

#include "resources/pattern.hpp"
#include "resources/terrain_library.hpp"

#include <cmath>
#include <algorithm>
#include <fstream>

namespace ts
{
    namespace world
    {
        Vector2u calculate_bitmap_size(const resources::Pattern& pattern);
    }
}

ts::Vector2u ts::world::calculate_bitmap_size(const resources::Pattern& pattern)
{
    Vector2u size = pattern.size();

    auto max_size = std::max(size.x, size.y);
    Vector2u result{ max_size, max_size };

    result.x += result.x >> 1; // +50% to allow free rotation
    if ((result.x & 63) != 0) {
        // Make the width a multiple of 32
        result.x |= 63;
        ++result.x;
    }

    result.y = result.x;
    return result;
}


ts::world::Collision_bitmap::Collision_bitmap(const std::shared_ptr<resources::Pattern>& pattern)
  : bitmap_size_(calculate_bitmap_size(*pattern)),
    bitmap_((bitmap_size_.x >> 6) * bitmap_size_.y, 0U),
    pattern_(pattern),
    rotation_()
{
    update({});
}

void ts::world::Collision_bitmap::set_rotation(Rotation<double> rotation)
{
    auto delta_rotation = rotation_ - rotation;
    if (std::abs(delta_rotation.degrees()) >= 1.0) {
        update(rotation);
        rotation_ = rotation;
    }
}

void ts::world::Collision_bitmap::update(Rotation<double> rotation)
{    
    Vector2i dest_center = bitmap_size_;
    dest_center.x >>= 1;
    dest_center.y >>= 1;

    const auto& source_pattern = *pattern_;
    Vector2i size = source_pattern.size(), source_center = size;
    source_center.x >>= 1;
    source_center.y >>= 1;

    auto inverse_rotation = -rotation;

    auto sin = static_cast<int>(std::sin(inverse_rotation.radians()) * 65536.0 + 0.5);
    auto cos = static_cast<int>(std::cos(inverse_rotation.radians()) * 65536.0 + 0.5);

    std::uint64_t* bitmap_ptr = &bitmap_[0];

    Vector2i point{ -dest_center.x, -dest_center.y };

    for (int y_end = point.y + bitmap_size_.y; point.y != y_end; ++point.y) {
        point.x = -dest_center.x;

        for (int x_end = point.x + bitmap_size_.x; point.x != x_end; ) {
            std::uint64_t entry = 0;

            for (auto entry_end = point.x + 64; point.x != entry_end; ++point.x) {
                Vector2i source_point
                { 
                    ((cos * point.x - sin * point.y + 0x8000) >> 16) + source_center.x,
                    ((cos * point.y + sin * point.x + 0x8000) >> 16) + source_center.y
                };

                entry <<= 1;
                if (source_point.x >= 0 && source_point.x < size.x &&
                    source_point.y >= 0 && source_point.y < size.y &&
                    source_pattern(source_point.x, source_point.y) != 0)
                {
                    entry |= 1;
                }
            }

            *bitmap_ptr++ = entry;
        }
    }
}

const ts::Vector2u& ts::world::Collision_bitmap::size() const
{
    return bitmap_size_;
}

const std::vector<std::uint64_t>& ts::world::Collision_bitmap::bitmap() const
{
    return bitmap_;
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

bool ts::world::Static_collision_bitmap::operator()(Vector2u point, std::size_t level) const
{
    if (point.x >= bitmap_size_.x || point.y >= bitmap_size_.y) return false;

    auto entry = bitmap_[point.y * (bitmap_size_.x >> 6) + (point.x >> 6)];

    return (entry << (point.x & 63) >> 63) != 0;
}

/*
bool ts::world::collision_test(const Collision_bitmap& subject, const Collision_bitmap& object)
{
    return false;
}
*/

ts::world::Collision_point ts::world::collision_test(const Collision_bitmap& subject, const Static_collision_bitmap& scenery,
                                                     Vector2i subject_position, std::size_t subject_level)                                                     
{
    Vector2i subject_size = subject.size();
    const auto& subject_bitmap = subject.bitmap();

    Vector2i scenery_size = scenery.size();
    const auto& scenery_bitmap = scenery.bitmap();    

    Vector2i subject_center(subject_size.x >> 1, subject_size.y >> 1);

    auto left_edge = subject_position.x - subject_center.x;
    auto top_edge = subject_position.y - subject_center.y;

    auto right_edge = std::min(left_edge + subject_size.x, scenery_size.x);
    auto bottom_edge = std::min(top_edge + subject_size.y, scenery_size.y);

    auto scenery_row_width = scenery_size.x >> 6;
    auto subject_row_width = subject_size.x >> 6;

    auto subject_top_edge = (top_edge >= 0 ? 0 : -top_edge);

    auto level_size = subject_row_width * scenery_size.y * subject_level;
    
    auto subject_row_begin = &subject_bitmap[level_size + subject_top_edge * subject_row_width];
    auto subject_begin = subject_row_begin;
    auto subject_end = subject_begin + subject_row_width;

    top_edge = std::max(top_edge, 0);
    auto scenery_row_begin = &scenery_bitmap[scenery_row_width * top_edge];
    auto scenery_begin = scenery_row_begin;
    auto scenery_end = scenery_row_begin + (right_edge >> 6);

    if (left_edge >= 0) {
        scenery_begin += left_edge >> 6;
    }

    else {
        subject_begin += -left_edge >> 6;
    }

    auto offset = (left_edge + subject_size.x) & 63;
    auto inverse_offset = (64 - offset) & 63;

    auto collision = [](std::int32_t base_x, std::int32_t y, std::uint64_t mask)
    {

        Collision_point result;
        result.collided = true;
        result.point = { base_x, y };

        while ((mask & (std::uint64_t(1) << 63)) == 0) {
            mask <<= 1;
            ++result.point.x;
        }

        return result;
    };

    auto base_x = (std::max(left_edge, 0) >> 6) << 6;

    for (auto scenery_y = top_edge; scenery_y != bottom_edge; ++scenery_y)
    {
        auto scenery_x = base_x;

        const auto* scenery_ptr = scenery_begin;
        const auto* subject_ptr = subject_begin;

        if (left_edge < 0)
        {
            if (auto mask = (inverse_offset ? (*subject_ptr << inverse_offset) & *scenery_ptr : 0))
            {
                return collision(scenery_x, scenery_y, mask);
            }

            ++subject_ptr;
        }

        else
        {            
            if (auto mask = (*subject_ptr >> offset) & *scenery_ptr)
                return collision(scenery_x, scenery_y, mask);

            ++scenery_ptr;
            scenery_x += 64;
        }

        for (; scenery_ptr != scenery_end; ++subject_ptr, ++scenery_ptr)
        {
            if (const auto scenery = *scenery_ptr) {
                auto mask = (inverse_offset ? (subject_ptr[0] << inverse_offset) & scenery : 0) |
                    ((*subject_ptr >> offset) & scenery);

                if (mask) {
                    return collision(scenery_x, scenery_y, mask);
                }

            }

            scenery_x += 64;
        }

        scenery_row_begin += scenery_row_width;
        subject_row_begin += subject_row_width;

        if (scenery_ptr != scenery_row_begin && subject_ptr != subject_row_begin)
        {
            if (auto mask = (inverse_offset ? (*subject_ptr << inverse_offset) & *scenery_ptr : 0))
            {
                return collision(scenery_x, scenery_y, mask);                
            }
                
        }

        scenery_begin += scenery_row_width;
        scenery_end += scenery_row_width;
        
        subject_begin += subject_row_width;
        subject_end += subject_row_width;
    }

    return Collision_point();
}
