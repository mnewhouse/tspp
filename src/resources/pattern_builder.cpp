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
#include "pattern_builder.hpp"
#include "track.hpp"

#include "pattern.hpp"

#include <SFML/Graphics.hpp>

namespace ts
{
    namespace resources
    {
        void apply_pattern(Pattern& dest, const Pattern& source, Int_rect rect, Vector2d position, Rotation<double> rotation);
    }
}


ts::resources::Pattern_builder::Pattern_builder(const Track& track)
: track_(track)
{
}

ts::resources::Pattern ts::resources::Pattern_builder::operator()(std::function<void()> step_operation)
{
    Pattern pattern(track_.size());

    auto apply_tile_func = [&](const Tile_group_definition& tile_def, const Level_tile& tile,
        const Tile_definition& sub_tile_def, const Level_tile& sub_tile,
        Vector2d position, Rotation<double> rotation)
    {
        const auto& pattern_file = sub_tile_def.pattern_file();
        const auto& resolved_path = resolve_include_path(pattern_file);

        auto handle = pattern_loader_.load_from_file(resolved_path);
        apply_pattern(pattern, *handle, sub_tile_def.pattern_rect, position, rotation);

        if (step_operation) step_operation();
    };

    const auto& tile_list = track_.tile_list();
    for_each_tile(tile_list.begin(), tile_list.end(), track_.tile_library(), apply_tile_func);

    return pattern;
}

void ts::resources::Pattern_builder::preload_pattern(const utf8_string& path)
{
    pattern_loader_.load_from_file(resolve_include_path(path));
}

const ts::utf8_string& ts::resources::Pattern_builder::resolve_include_path(const utf8_string& path)
{
    auto it = precomputed_paths_.find(path);
    if (it == precomputed_paths_.end())
    {
        auto resolved_path = track_.find_include_path(path);
        it = precomputed_paths_.emplace(path, std::move(resolved_path)).first;
    }

    return it->second;
}


void ts::resources::apply_pattern(Pattern& dest, const Pattern& source, 
                                  Int_rect rect, Vector2d position, Rotation<double> rotation)
{
    double sangle = -std::sin(rotation.radians());
    double cangle = std::cos(rotation.radians());

    Vector2i world_size = dest.size();    
    Vector2i pattern_size = source.size();
    Vector2i source_size(rect.width, rect.height);

    auto dest_size = [=]()
    {
        auto x = double(source_size.x >> 1);
        auto y = double(source_size.y >> 1);

        auto cx = x * cangle;
        auto cy = y * cangle;
        auto sx = x * sangle;
        auto sy = y * sangle;

        auto width = std::ceil(std::abs(cx) + std::abs(sy));
        auto height = std::ceil(std::abs(cy) + std::abs(sx));

        return Vector2i(std::int32_t(width) << 1, std::int32_t(height) << 1);
    }();

    auto isin = std::int32_t(65536.0 * sangle);
    auto icos = std::int32_t(65536.0 * cangle);

    std::int32_t cx = dest_size.x >> 1;
    std::int32_t cy = dest_size.y >> 1;

    Vector2i int_pos(static_cast<std::int32_t>(position.x + 0.5), static_cast<std::int32_t>(position.y + 0.5));

    std::int32_t xd = (source_size.x - dest_size.x) << 15;
    std::int32_t yd = (source_size.y - dest_size.y) << 15;
    std::int32_t ax = (cx << 16) - (icos * cx);
    std::int32_t ay = (cy << 16) - (isin * cx);

    std::int32_t base_x = int_pos.x - cx;
    std::int32_t base_y = int_pos.y - cy;

    std::int32_t start_x = clamp(base_x, 0, world_size.x);
    std::int32_t start_y = clamp(base_y, 0, world_size.y);
    
    std::int32_t end_x = clamp(base_x + dest_size.x + 1, 0, world_size.x);
    std::int32_t end_y = clamp(base_y + dest_size.y + 1, 0, world_size.y);

    if (rect.right() >= pattern_size.x) rect.width = pattern_size.x - rect.left;
    if (rect.bottom() >= pattern_size.y) rect.height = pattern_size.y - rect.top;
    
    for (std::int32_t dest_y = start_y, y = dest_y - base_y; dest_y != end_y; ++y, ++dest_y)
    {
        std::int32_t sdx = (ax + (isin * (cy - y))) + xd + 0x8000;
        std::int32_t sdy = (ay - (icos * (cy - y))) + yd + 0x8000;

        // Get the range in which (0 <= source_x < rect.width AND 0 <= source_y < rect.height)

        auto determine_range = [](std::int32_t base_value, std::int32_t multiplier, std::int32_t min, std::int32_t max)
            -> std::pair<std::int32_t, std::int32_t>
        {
            if (multiplier == 0)
            {
                auto value = base_value >> 16;
                if (value < min || value >= max) return std::make_pair(0, 0); // Empty range

                // Infinite range
                return std::make_pair(std::numeric_limits<std::int32_t>::min(),
                                      std::numeric_limits<std::int32_t>::max());
            }

            auto equal_range = [multiplier, base_value](std::int32_t value)
            {
                auto target_values = std::make_pair((value << 16) - base_value,
                                                    ((value << 16) + 0xFFFF) - base_value);

                auto div = std::make_pair(std::div(target_values.first, multiplier),
                                          std::div(target_values.second, multiplier));

                if (div.first.rem != 0 && target_values.first > 0)
                {
                    div.first.quot += (multiplier < 0 ? -1 : 1);
                }

                if (div.second.rem != 0 && target_values.second < 0)
                {
                    div.second.quot += (multiplier < 0 ? 1 : -1);
                }

                if (div.first.quot > div.second.quot)
                {
                    std::swap(div.first.quot, div.second.quot);
                }

                return std::make_pair(div.first.quot, div.second.quot);
            };

            auto min_range = equal_range(min);
            auto max_range = equal_range(max);

            auto begin = min_range.first;
            auto end = max_range.first;

            assert(((base_value + min_range.first * multiplier) >> 16) == min);
            assert(((base_value + min_range.second * multiplier) >> 16) == min);

            assert(((base_value + max_range.first * multiplier) >> 16) == max);
            assert(((base_value + max_range.second * multiplier) >> 16) == max);

            if (begin > end)
            {
                begin = max_range.second + 1;
                end = min_range.second + 1;
            }

            return std::make_pair(begin, end);
        };

        auto x_range = determine_range(sdx, icos, 0, rect.width);
        auto y_range = determine_range(sdy, isin, 0, rect.height);

        // Get the overlap of the ranges
        auto overlap_range = std::make_pair(std::max(x_range.first, y_range.first),
                                            std::min(x_range.second, y_range.second));

        auto range = std::make_pair(std::max(overlap_range.first + base_x, start_x),
                                    std::min(overlap_range.second + base_x, end_x));

        range.second = std::max(range.first, range.second);

        auto row_begin = dest.row_begin(dest_y);
        auto dest_start = row_begin + range.first;
        auto dest_end = row_begin + range.second;

        auto skipped = range.first - base_x;
        sdx += skipped * icos;
        sdy += skipped * isin;

        // Loop invariant: 0 <= source_x < rect.width && 0 <= source_y < rect.height
        for (auto dest_ptr = dest_start; dest_ptr != dest_end; ++dest_ptr, sdx += icos, sdy += isin)
        {
            std::int32_t source_x = sdx >> 16;
            std::int32_t source_y = sdy >> 16;

            assert(source_x >= 0 && source_x < rect.width);
            assert(source_y >= 0 && source_y < rect.height);

            auto terrain = source(source_x + rect.left, source_y + rect.top);
            if (terrain != 0)
            {
                *dest_ptr = terrain;
            }
        }
    }
}