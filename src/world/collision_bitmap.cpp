/*
 * Turbo Sliders++
 * Copyright (C) 2013-2014 Martin Newhouse
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "stdinc.hpp"

#include "collision_bitmap.hpp"
#include "collision_helpers.hpp"

#include "resources/pattern.hpp"
#include "resources/terrain_library.hpp"

namespace ts
{
    namespace world
    {
        Vector2u calculate_bitmap_size(const resources::Pattern& pattern);

        std::vector<std::uint32_t> generate_rotated_bitmaps(const resources::Pattern& pattern, Vector2i bitmap_size, std::size_t num_rotations);

        Collision_point collision_test_impl(const std::uint32_t* subject_data, const std::uint32_t* object_data,
                                            Vector2i subject_size, Vector2i object_size,
                                            Vector2i subject_position, Vector2i object_position,
                                            Vector2i subject_center, Vector2i object_center);
    }
}

ts::Vector2u ts::world::calculate_bitmap_size(const resources::Pattern& pattern)
{
    Vector2u size = pattern.size();

    if ((size.x & 31) != 0) {
        // Make the width a multiple of 32
        size.x |= 31;
        ++size.x;
    }

    return size;
}

std::vector<std::uint32_t> ts::world::generate_rotated_bitmaps(const resources::Pattern& pattern, Vector2i bitmap_size, std::size_t num_rotations)
{
    std::vector<std::uint32_t> bitmap(bitmap_size.y * (bitmap_size.x >> 5) * num_rotations);

    Vector2i pattern_size = pattern.size();

    Vector2i dest_center(bitmap_size.x >> 1, bitmap_size.y >> 1);

    Vector2<double> source_center(pattern_size.x * 0.5, pattern_size.y * 0.5);
    Rotation<double> rotation, increment = Rotation<double>::degrees(360.0 / num_rotations);

    auto row_width = bitmap_size.x >> 5;

    for (std::int32_t r = 0; r != num_rotations; ++r, rotation += increment)
    {
        auto sin = std::sin(rotation.radians());
        auto cos = std::cos(rotation.radians());

        auto frame_ptr = &bitmap[r * row_width * bitmap_size.y];

        for (std::int32_t y = 0; y != bitmap_size.y; ++y)
        {
            for (std::int32_t x = 0; x < bitmap_size.x; )
            {
                for (std::int32_t x_end = x + 32; x != x_end; ++x)
                {
                    Vector2i dest_point(x, y);

                    auto source_point = transform_point(dest_point - dest_center, -sin, cos) + source_center;

                    auto source_x = static_cast<std::int32_t>(std::round(source_point.x));
                    auto source_y = static_cast<std::int32_t>(std::round(source_point.y));

                    if (source_x >= 0 && source_y >= 0 && source_x < pattern_size.x && source_y < pattern_size.y &&
                        pattern(source_x, source_y) != 0)
                    {
                        auto* entry = frame_ptr + (row_width * dest_point.y) + (dest_point.x >> 5);
                        *entry |= 1 << (31 - (dest_point.x & 31));
                    }
                }            
            }
        }
    }

    return bitmap;
}


ts::world::Collision_bitmap::Collision_bitmap(const resources::Pattern& pattern, std::size_t num_rotations)
  : bitmap_size_(calculate_bitmap_size(pattern)),
    bitmap_(generate_rotated_bitmaps(pattern, bitmap_size_, num_rotations)),
    num_rotations_(num_rotations)
{
}

const ts::Vector2u& ts::world::Collision_bitmap::size() const
{
    return bitmap_size_;
}

const std::vector<std::uint32_t>& ts::world::Collision_bitmap::bitmap() const
{
    return bitmap_;
}

std::size_t ts::world::Collision_bitmap::rotation_index(Rotation<double> rotation) const
{
    std::int32_t num_rotations = num_rotations_;

    const double frame_factor = num_rotations / 360.0;
    std::int32_t frame_id = static_cast<std::int32_t>(std::floor(rotation.degrees() * frame_factor + 0.5)) + num_rotations;
    if (frame_id >= num_rotations) frame_id -= num_rotations;

    return frame_id;
}

bool ts::world::Collision_bitmap::operator()(Vector2i point, std::size_t frame_index) const
{
    auto row_width = bitmap_size_.x >> 5;
    auto base_ptr = &bitmap_[bitmap_size_.y * row_width * frame_index];
    auto entry = base_ptr + point.y * row_width + (point.x >> 5);

    return (*entry << (point.x & 31) >> 31) != 0;
}

bool ts::world::Collision_bitmap::operator()(Vector2i point, Rotation<double> rotation) const
{
    return operator()(point, rotation_index(rotation));
}

ts::world::Static_collision_bitmap::Static_collision_bitmap(const resources::Pattern& pattern, 
                                                            const resources::Terrain_library& terrain_lib, std::size_t num_levels)
  : num_levels_(num_levels),
    bitmap_size_((((pattern.size().x - 1) >> 5) + 1) << 5, pattern.size().y)
{
    auto pattern_size = pattern.size();
    auto row_width = bitmap_size_.x >> 5;

    bitmap_.resize(row_width * bitmap_size_.y * std::max<std::size_t>(num_levels, 1));

    auto is_wall = [&](std::uint32_t x, std::uint32_t y, std::size_t level)
    {
        auto terrain_id = pattern(x, y);

        terrain_id = terrain_lib.sub_terrain(terrain_id, level);
        return terrain_lib.terrain_by_id(terrain_id).is_wall;
    };

    std::vector<bool> terrain_buffer(256, false);

    for (std::size_t level = 0; level != num_levels; ++level) 
    {
        for (std::uint32_t terrain_id = 0; terrain_id != 256; ++terrain_id)
        {
            auto sub_terrain = terrain_lib.sub_terrain(terrain_id, level);
            if (terrain_lib.terrain_by_id(sub_terrain).is_wall)
            {
                terrain_buffer[terrain_id] = true;
            }
        }

        std::uint32_t* level_ptr = &bitmap_[level * row_width * bitmap_size_.y];
        auto pattern_it = pattern.begin();

        for (std::uint32_t y = 0; y != pattern_size.y; ++y) 
        {
            auto row_ptr = level_ptr + y * row_width;

            for (std::uint32_t x = 0; x != pattern_size.x; ++x)
            {
                if (terrain_buffer[*pattern_it++])
                {
                    auto entry_ptr = row_ptr + (x >> 5);
                    *entry_ptr |= 1 << (31 - (x & 31));
                }
            }
        }
    }
}

const ts::Vector2u& ts::world::Static_collision_bitmap::size() const
{
    return bitmap_size_;
}


const std::vector<std::uint32_t>& ts::world::Static_collision_bitmap::bitmap() const
{
    return bitmap_;
}

bool ts::world::Static_collision_bitmap::operator()(const Vector2u& point, std::uint32_t level) const
{
    auto entry = bitmap_[point.y * (bitmap_size_.x >> 5) + (point.x >> 5)];

    return (entry << (point.x & 31) >> 31) != 0;
}

ts::world::Collision_point ts::world::collision_test_impl(const std::uint32_t* subject_data, const std::uint32_t* object_data,
                                                          Vector2i subject_size, Vector2i object_size,
                                                          Vector2i subject_position, Vector2i object_position,
                                                          Vector2i subject_center, Vector2i object_center)
{
    Int_rect subject_area(subject_position.x - subject_center.x, subject_position.y - subject_center.y,
                           subject_size.x, subject_size.y);

    Int_rect object_area(object_position.x - object_center.x, object_position.y - object_center.y,
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

    auto subject_offset = static_cast<std::uint32_t>(subject_area.left) & 31;
    auto object_offset = static_cast<std::uint32_t>(object_area.left) & 31;
    auto intersect_offset = static_cast<std::uint32_t>(intersect_area.left) & 31;

    auto subject_inverse_offset = (32 - subject_offset) & 31;
    auto object_inverse_offset = (32 - object_offset) & 31;    

    auto subject_row_width = subject_size.x >> 5;
    auto object_row_width = object_size.x >> 5;

    auto x_begin = intersect_area.left >> 5 << 5;
    auto x_end = intersect_area.right();
    if ((x_end & 31) != 0) x_end = (x_end | 31) + 1;

    auto subject_row_begin = subject_data + (subject_row_width * subject_local_area.top);
    auto object_row_begin = object_data + (object_row_width * object_local_area.top);

    auto subject_begin = subject_row_begin;
    if (x_begin >= subject_area.left) subject_begin += (x_begin - subject_area.left) >> 5;
    else --subject_begin;

    auto object_begin = object_row_begin;
    if (x_begin >= object_area.left) object_begin += (x_begin - object_area.left) >> 5;
    else --object_begin;

    for (auto y = intersect_area.top; y != intersect_area.bottom(); ++y)
    {
        auto subject_ptr = subject_begin;
        auto object_ptr = object_begin;

        auto subject_row_end = subject_row_begin + subject_row_width;
        auto object_row_end = object_row_begin + object_row_width;

        for (auto x = x_begin; x != x_end; x += 32)
        {
            std::uint32_t subject_mask = 0, object_mask = 0;

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

            if (object_offset) 
            {
                if (object_ptr >= object_row_begin)
                    object_mask |= *object_ptr << object_inverse_offset;

                if (++object_ptr < object_row_end)
                    object_mask |= *object_ptr >> object_offset;
            }

            else 
            {
                object_mask |= *object_ptr++;
            }

            if (auto mask = subject_mask & object_mask)
            {
                Collision_point result;
                result.collided = true;
                result.point.x = x; result.point.y = y;

                while ((mask & (std::uint32_t(1) << 31)) == 0)
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
                                                     std::size_t subject_rotation_index, std::size_t object_rotation_index)
{    
    const auto& subject_bitmap = subject.bitmap();
    const auto& object_bitmap = object.bitmap();

    Vector2i subject_size = subject.size();
    Vector2i object_size = object.size();
   
    auto subject_data_size = subject_size.y * (subject_size.x >> 5);
    auto object_data_size = object_size.y * (object_size.x >> 5);

    Vector2i subject_center(subject_size.x >> 1, subject_size.y >> 1);
    Vector2i object_center(object_size.x >> 1, object_size.y >> 1);

    auto subject_data = &subject_bitmap[subject_rotation_index * subject_data_size];
    auto object_data = &object_bitmap[object_rotation_index * object_data_size];

    return collision_test_impl(subject_data, object_data, subject_size, object_size, 
                               subject_position, object_position, subject_center, object_center);
}

ts::world::Collision_point ts::world::collision_test(const Collision_bitmap& subject, const Static_collision_bitmap& scenery,
                                                     Vector2i subject_position, std::size_t subject_rotation_index, std::size_t subject_level)
{
    const auto& bitmap = subject.bitmap();
    const auto& scenery_bitmap = scenery.bitmap();
    Vector2i subject_size = subject.size();
    Vector2i scenery_size = scenery.size();

    Vector2i subject_center(subject_size.x >> 1, subject_size.y >> 1);

    Vector2i scenery_center(0, 0);
    Vector2i scenery_position(0, 0);

    auto scenery_data_size = scenery_size.y * (scenery_size.x >> 5);
    auto subject_data_size = subject_size.y * (subject_size.x >> 5);

    auto subject_data = &bitmap[subject_rotation_index * subject_data_size];
    auto scenery_data = &scenery_bitmap[subject_level * scenery_data_size];

    return collision_test_impl(subject_data, scenery_data, subject_size, scenery_size, 
                               subject_position, scenery_position, subject_center, scenery_center);    
}
