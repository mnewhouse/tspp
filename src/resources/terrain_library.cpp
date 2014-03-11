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

#include "terrain_library.hpp"

#include <iostream>

void ts::resources::Terrain_library::define_terrain(const Terrain_definition& terrain_definition)
{    
    auto id = terrain_definition.id;
    terrains_[id] = terrain_definition;

    define_sub_terrain(id, id, 0);
}

void ts::resources::Terrain_library::define_sub_terrain(Terrain_id terrain, Terrain_id sub_id,
                                                        std::size_t level_start, std::size_t level_count)
{
    for (unsigned i = 0; i != level_count; ++i) {
        define_sub_terrain(terrain, sub_id, level_start++);
    }
}

void ts::resources::Terrain_library::define_sub_terrain(Terrain_id terrain, Terrain_id sub_id, std::size_t level)
{
    auto key = std::make_pair(terrain, level);
    sub_terrains_[key] = sub_id;
}

ts::resources::Terrain_id ts::resources::Terrain_library::sub_terrain(Terrain_id terrain, std::size_t level) const
{
    auto key = std::make_pair(terrain, level);

    auto it = sub_terrains_.find(key);
    if (it == sub_terrains_.end()) {
        return 0;
    }

    return it->second;
}


const ts::resources::Terrain_definition& ts::resources::Terrain_library::terrain_by_id(Terrain_id id) const
{
    return terrains_[id];
}