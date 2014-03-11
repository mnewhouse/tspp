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

#include "tile_library.hpp"
#include <iostream>

void ts::resources::Tile_library::define_tile(const Tile_definition& tile_def)
{
    auto it = tile_map_.find(tile_def.id);
    if (it == tile_map_.end()) {
        tile_map_.insert(std::make_pair(tile_def.id, tile_def));
    }
    else {
        it->second = tile_def;
    }

    Tile_group_definition tile_group_def(tile_def.id, 1);

    Level_tile sub_tile;
    sub_tile.id = tile_def.id;

    tile_group_def.add_sub_tile(sub_tile);
    define_tile_group(tile_group_def);
}

void ts::resources::Tile_library::define_tile_group(const Tile_group_definition& tile_group_def)
{
    auto id = tile_group_def.id();
    auto it = tile_group_map_.find(id);
    if (it == tile_group_map_.end()) {
        tile_group_map_.insert(std::make_pair(id, tile_group_def));
    }

    else {
        it->second = tile_group_def;
    }
}


const ts::resources::Tile_definition* ts::resources::Tile_library::tile(Tile_id id) const
{
    auto it = tile_map_.find(id);
    if (it == tile_map_.end()) return nullptr;

    return &it->second;
}

const ts::resources::Tile_group_definition* ts::resources::Tile_library::tile_group(Tile_id id) const
{
    auto it = tile_group_map_.find(id);
    if (it == tile_group_map_.end()) return nullptr;

    return &it->second;
}
