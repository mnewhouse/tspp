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

#include "terrain_map.hpp"
#include "track.hpp"

#include "pattern_loader.hpp"
#include "pattern.hpp"

#include <random>
#include <SFML/Graphics.hpp>

namespace ts
{
    namespace resources 
    {
        void apply_pattern(Pattern& dest, const Pattern& source, Int_rect rect, Vector2d position, Rotation<double> rotation);
    }
}


ts::resources::Pattern ts::resources::build_track_pattern(const resources::Track& track)
{
    Pattern_loader pattern_loader;
    Pattern pattern(track.size());

    auto place_tile_func = [&](const Tile_group_definition& tile_def, const Level_tile& tile,
                               const Tile_definition& sub_tile_def, const Level_tile& sub_tile,
                               Vector2d position, Rotation<double> rotation)
    {
        auto pattern_file = track.find_include_path(sub_tile_def.pattern_file());

        auto handle = pattern_loader.load_from_file(pattern_file);
        apply_pattern(pattern, *handle, sub_tile_def.pattern_rect, position, rotation);
    };

    for_each_tile(track.tile_list().begin(), track.tile_list().end(), track.tile_library(), place_tile_func);

    return pattern;
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

        return Vector2i(int(width) << 1, int(height) << 1);
    }();

    auto isin = std::int32_t(65536.0 * sangle);
    auto icos = std::int32_t(65536.0 * cangle);

    std::int32_t cx = dest_size.x >> 1;
    std::int32_t cy = dest_size.y >> 1;

    Vector2i int_pos(int(position.x + 0.5), int(position.y + 0.5));

    std::int32_t xd = (source_size.x - dest_size.x) << 15;
    std::int32_t yd = (source_size.y - dest_size.y) << 15;
    std::int32_t ax = (cx << 16) - (icos * cx);
    std::int32_t ay = (cy << 16) - (isin * cx);
    
    for (std::int32_t y = 0; y <= dest_size.y; y++) {
        auto dest_y = y + int_pos.y - cy;
        if (dest_y < 0 || dest_y >= world_size.y) continue;

        std::int32_t sdx = (ax + (isin * (cy - y))) + xd;
        std::int32_t sdy = (ay - (icos * (cy - y))) + yd;

        for (std::int32_t x = 0; x <= dest_size.x; x++, sdx += icos, sdy += isin)
        {
            auto dest_x = x + int_pos.x - cx;;
            if (dest_x < 0 || dest_x >= world_size.x) continue;

            std::int32_t source_x = (sdx + 0x8000) >> 16;
            std::int32_t source_y = (sdy + 0x8000) >> 16;

            if (source_x >= 0 && source_x < rect.width && 
                source_y >= 0 && source_y < rect.height)
            {             

                source_x += rect.left;
                source_y += rect.top;

                if (source_x >= 0 && source_x < pattern_size.x &&
                    source_y >= 0 && source_y < pattern_size.y)
                {
                    auto terrain = source(source_x, source_y);
                    if (terrain != 0) {
                        dest(dest_x, dest_y) = terrain;
                    }                    
                }
            } 
        }
    }
}