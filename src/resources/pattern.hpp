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

#ifndef RESOURCES_PATTERN_FILE_HPP
#define RESOURCES_PATTERN_FILE_HPP

#include <vector>
#include <cstdint>

#include "core/rect.hpp"
#include "core/vector2.hpp"

#include "collision_mask.hpp"
#include "terrain_definition.hpp"
#include "terrain_library.hpp"

namespace ts
{
    namespace resources
    {
        class Pattern
        {
        public:
            explicit Pattern(Vector2u size = Vector2u{});
            explicit Pattern(const std::string& file_name, Int_rect rect = Int_rect{});

            void load_from_file(const std::string& file_name, Int_rect rect);

            const Terrain_id& operator()(std::size_t x, std::size_t y) const;
            Terrain_id& operator()(std::size_t x, std::size_t y);

            Vector2u size() const;

        private:
            Vector2u size_;
            std::vector<Terrain_id> bytes_;
        };

        /*

        template <typename Callback>
        void generate_polygons_from_pattern(Pattern pattern, Callback callback_func, Int_rect rect = Int_rect{});

        template <typename Callback>
        void generate_polygons_from_pattern(Pattern pattern, const Terrain_library& terrain_lib,                                            Callback callback_func, Int_rect rect = Int_rect{});


        namespace impl 
        {
            Collision_polygon generate_polygon(Pattern& pattern, Int_rect rect, Vector2i start_point);
        }
        */
    }
}

/*
template <typename Callback>
void ts::resources::generate_polygons_from_pattern(Pattern pattern, Callback callback_func, Int_rect rect)
{
    if (rect.width == 0) {
        rect.width = pattern.size().x;
        rect.left = 0;
    }

    if (rect.height == 0) {
        rect.height = pattern.size().y;
        rect.top = 0;
    }

    for (int y = rect.top; y != rect.bottom(); ++y) {
        for (int x = rect.left; x != rect.right(); ++x) {
            auto terrain_id = pattern(x, y);
            if (terrain_id) {
                auto polygons = impl::generate_polygon(pattern, rect, Vector2i(x, y));
                callback_func(std::move(polygons), terrain_id);
            }
        }
    }
}


template <typename Callback>
void ts::resources::generate_polygons_from_pattern(Pattern pattern, const Terrain_library& terrain_lib, 
                                                   Callback callback_func, Int_rect rect)
{
    if (rect.width == 0) {
        rect.width = pattern.size().x;
        rect.left = 0;
    }

    if (rect.height == 0) {
        rect.height = pattern.size().y;
        rect.top = 0;
    }

    for (int y = rect.top; y != rect.bottom(); ++y) {
        for (int x = rect.left; x != rect.right(); ++x) {
            auto terrain_id = pattern(x, y);
            if (terrain_id) {
                const auto& terrain_def = terrain_lib.terrain_by_id(terrain_id);

                if (terrain_def.is_wall) {
                    auto polygons = impl::generate_polygon(pattern, rect, Vector2i(x, y));
                    callback_func(std::move(polygons), terrain_id);
                }
            }
        }
    }
}
*/

#endif