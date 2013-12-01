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

#ifndef RESOURCES_TILE_LIBRARY_HPP
#define RESOURCES_TILE_LIBRARY_HPP

#include "tile.hpp"

#include <map>

#include <SFML/Graphics/Transformable.hpp>

namespace ts
{

    namespace resources
    {
        // The tile library keeps track of all the tiles and tile groups that have been defined during
        // the track loading process, allowing for relatively efficient retrieval later on.
        class Tile_library
        {
        public:
            void define_tile(const Tile_definition& tile_def);
            void define_tile_group(const Tile_group_definition& tile_group_def);

            const Tile_definition* tile(Tile_id id) const;
            const Tile_group_definition* tile_group(Tile_id id) const;

        private:
            std::map<Tile_id, Tile_definition> tile_map_;
            std::map<Tile_id, Tile_group_definition> tile_group_map_;
        };


        template <typename IteratorType, typename F>
        void for_each_tile(IteratorType begin, IteratorType end, const Tile_library& tile_library, F func);
    }
}

template <typename IteratorType, typename F>
void ts::resources::for_each_tile(IteratorType begin, IteratorType end, const Tile_library& tile_library, F func)
{
    for (auto it = begin; it != end; ++it) {
        const auto& tile = *it;
        auto tile_def = tile_library.tile_group(tile.id);
        if (!tile_def) continue;

        for (auto& sub_tile : tile_def->sub_tiles()) {
            auto sub_tile_def = tile_library.tile(sub_tile.id);
            if (!sub_tile_def) continue;

            sf::Transformable transformable;
            transformable.setRotation(float(tile.rotation.degrees()));

            const auto& transform = transformable.getTransform();
            auto position = transform.transformPoint(float(sub_tile.position.x), float(sub_tile.position.y));
            auto sub_tile_rotation = tile.rotation + sub_tile.rotation;

            auto sub_tile_position = tile.position;
            sub_tile_position.x += position.x;
            sub_tile_position.y += position.y;

            func(*tile_def, tile, *sub_tile_def, sub_tile, sub_tile_position, sub_tile_rotation);
        }
    }
}

#endif