/*
 * Turbo Sliders++
 * Copyright (C) 2013 Martin Newhouse
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

#include "pattern.hpp"

extern "C" {
#include "png/png.h"
#include "png/pngstruct.h"
}

#include <cstdio>
#include <algorithm>
#include <iostream>

#include "core/utility.hpp"

#include "collision_mask.hpp"

ts::resources::Pattern::Pattern(const std::string& file_name, Int_rect rect)
{
    load_from_file(file_name, rect);
}

ts::resources::Pattern::Pattern(Vector2u size)
: bytes_(size.x * size.y, 0), size_(size)
{
}

void ts::resources::Pattern::load_from_file(const std::string& file_name, Int_rect rect)
{
    auto file_handle = std::fopen(file_name.c_str(), "rb");
    if (!file_handle) {
        throw std::runtime_error("could not open pattern file " + file_name + ".");
    }

    auto destroy_file_handle = scope_exit([file_handle]() { std::fclose(file_handle); });    

    auto read_png_file = [this, &rect](FILE* file)
    {
        unsigned char sig[8] = {};

        std::fread(sig, 1, 8, file);
        if (!png_check_sig(sig, 8)) return false;

        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png_ptr) return false;

        auto destroy_read_struct = scope_exit([&]() { png_destroy_read_struct(&png_ptr, nullptr, nullptr); });

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) return false;

        if (setjmp(png_jmpbuf(png_ptr))) return false;

        auto destroy_info_struct = scope_exit([&]() { png_destroy_info_struct(png_ptr, &info_ptr); });

        png_init_io(png_ptr, file);
        png_set_sig_bytes(png_ptr, 8);
        png_read_info(png_ptr, info_ptr);

        if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_PALETTE)
            return false;

        auto image_width = png_get_image_width(png_ptr, info_ptr);
        auto image_height = png_get_image_height(png_ptr, info_ptr);

        std::vector<Terrain_id> byte_array(image_width * image_height);
        std::vector<png_bytep> row_pointers(image_height);


        {
            std::size_t row = 0;
            std::generate(row_pointers.begin(), row_pointers.end(), 
                [&]() { return &byte_array[row++ * image_width]; }
            );
        }

        png_read_image(png_ptr, &row_pointers[0]);
        png_read_end(png_ptr, nullptr);

        if (rect.width == 0) {
            rect.width = image_width;
            rect.left = 0;
        }

        if (rect.height == 0) {
            rect.height = image_height;
            rect.top = 0;
        }

        if (image_width == rect.width && rect.left == 0 &&
            image_height == rect.height && rect.top == 0) 
        {
            bytes_ = std::move(byte_array);
        }

        else {
            for (auto y = rect.top; y != rect.bottom(); ++y) {
                for (auto x = rect.left; x != rect.right(); ++x) {
                    auto idx = x + y * image_width;
                    (*this)(x, y) = byte_array[idx];
                }
            }            
        }

        size_.x = rect.width;
        size_.y = rect.height;
        
        return true;
    };

    if (!read_png_file(file_handle)) {
        throw std::runtime_error("invalid pattern file; must be an indexed PNG image");
    }
}

/*
ts::resources::Collision_polygon ts::resources::impl::generate_polygon(Pattern& pattern, Int_rect rect, Vector2i start_point)
{
    auto terrain_id = pattern(start_point.x, start_point.y);

    using Point = Vector2i;
    std::vector<Point> left_points;
    left_points.reserve(16);
    left_points.push_back(start_point);

    std::vector<Point> right_points;
    right_points.reserve(16);
    right_points.push_back(start_point);

    right_points.push_back([&pattern, &rect, terrain_id, start_point]()
    {
        auto result = start_point;
        for (; result.x < rect.right() && pattern(result.x, result.y) == terrain_id; ++result.x);

        return result;
    }());

    for (auto x = start_point.x; x <= right_points.back().x; ++x) {
        pattern(x, start_point.y) = 0;
    }

    auto range_start = left_points.back().x - 1;
    auto range_end = right_points.back().x + 2;

    auto pending_left = left_points.back();
    auto pending_right = right_points.back();

    Vector2i left_direction(-1, 0);
    Vector2i right_direction(1, 0);

    for (auto y = start_point.y + 1, bottom = rect.bottom(); y < bottom; ++y) {
        auto get_extreme = [&rect, y]
            (Point reference_point, Vector2i direction) -> int
        {
            if (direction.y == 0) {
                if (direction.x < 0) return 0;

                return rect.right();
            }

            return reference_point.x + direction.x * (y - reference_point.y) / direction.y;
        };

        // Greedily eat as many pixels as a convex polygon allows.
        auto longest_sub_sequence = [&pattern, &rect, terrain_id, y](int min_x, int max_x)
        {
            int sequence_length = 0;
            int sequence_start = min_x;

            auto ptr = &pattern(0, y);
            for (auto x = min_x; x < max_x; ++x) {
                auto it = std::find_if(ptr + x, ptr + max_x, 
                    [terrain_id](Terrain_id id) { return id != terrain_id; });

                auto count = int(it - (ptr + x));
                if (count > sequence_length) {
                    sequence_length = count;
                    sequence_start = x;
                }

                x += count;
            }

            return std::make_pair(sequence_start, sequence_start + sequence_length);
        };

        auto min_x = get_extreme(left_points.back(), left_direction);
        auto max_x = get_extreme(right_points.back(), right_direction);

        auto sub_sequence = longest_sub_sequence(std::max(range_start, min_x), std::min(range_end, max_x));
        if (sub_sequence.second == sub_sequence.first) {
            break;
        }



        for (auto& x = sub_sequence.first; x >= min_x && pattern(x, y) == terrain_id; --x);
        for (auto& x = sub_sequence.second; x < max_x && pattern(x, y) == terrain_id; ++x);

        ++sub_sequence.first;

        auto get_pending_x = [](Vector2i pending_point, Vector2i last_point, int y)
        {
            auto direction = pending_point - last_point;
            if (direction.y == 0) return last_point.x;

            return last_point.x + direction.x * (y - last_point.y) / direction.y;
        };

        Vector2i left{ sub_sequence.first, y };
        Vector2i right{ sub_sequence.second, y };

        auto last_left = left_points.back();
        auto last_right = right_points.back();

        if (left_direction.y == 0) {            
            left_direction = left - last_left;
            left_points.push_back(left);
        }

        else {
            // Get the x point on the 'pending' line
            auto pending_x = get_pending_x(pending_left, last_left, y);

            if (left.x > pending_x) {
                // Add new point
                left_direction = pending_left - last_left;
                left_points.push_back(pending_left);
            }

            if (left.x != pending_left.x) {
                left_direction = left - left_points.back();
            }
        }

        if (right_direction.y == 0) {
            right_direction = right - last_right;
            right_points.push_back(right);
        }

        else {
            auto pending_x = get_pending_x(pending_right, last_right, y);

            if (right.x < pending_x) {
                // Add new point
                right_direction = pending_right - last_right;
                right_points.push_back(pending_right);
            }

            if (right.x != pending_right.x) {
                right_direction = right - right_points.back();
            }
        }

        pending_left = left;
        pending_right = right;

        range_start = left.x - 1;
        range_end = right.x + 1;

        for (auto x = left.x; x != right.x; ++x) {
            pattern(x, y) = 0;
        }
    }

    left_points.push_back(pending_left);
    right_points.push_back(pending_right);

    Collision_polygon polygon;
    auto& points = polygon.points;
    points.reserve(left_points.size() + right_points.size() + 2);

    auto left_it = std::adjacent_find(left_points.begin(), left_points.end(), 
        [](const Point& a, const Point& b) { return (b - a).x > 0; });

    auto right_it = std::adjacent_find(right_points.begin(), right_points.end(), 
        [](const Point& a, const Point& b) { return (b - a).x < 0; });

    auto inc_y = [](Point point) { ++point.y; return point; };

    std::copy(left_points.begin(), left_it, std::back_inserter(points));
    if (left_it != left_points.end()) {
        points.push_back(*left_it);
        std::transform(left_it, left_points.end(), std::back_inserter(points), inc_y);
    }

    if (right_it != right_points.end()) {
        auto point = *right_it;
        std::transform(right_it, right_points.end(), right_it, inc_y);
        std::reverse_copy(right_it, right_points.end(), std::back_inserter(points));
        points.push_back(point);
    }    

    std::reverse_copy(right_points.begin(), right_it, std::back_inserter(points));

    
    if (start_point.x == 159 && start_point.y == 165) {
        std::cout << "poly\n";
        for (auto pt : points) {
            std::cout << pt << "\n";
        }
    }
    

    return polygon;
}
*/


const ts::resources::Terrain_id& ts::resources::Pattern::operator()(std::size_t x, std::size_t y) const
{
    return bytes_[x + y * size().x];
}

ts::resources::Terrain_id& ts::resources::Pattern::operator()(std::size_t x, std::size_t y)
{
    return bytes_[x + y * size().x];
}

ts::Vector2u ts::resources::Pattern::size() const
{
    return size_;
}