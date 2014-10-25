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

#pragma once

#ifndef RESOURCES_TERRAIN_LIBRARY_HPP
#define RESOURCES_TERRAIN_LIBRARY_HPP

#include "terrain_definition.hpp"

namespace ts
{
    namespace resources
    {
        // The terrain library stores all the terrains and sub-terrains, and provides functions
        // to obtain information about said terrains and sub-terrains.

        class Terrain_library
        {
        public:
            Terrain_library();

            void define_terrain(const Terrain_definition& terrain_definition);
            void define_sub_terrain(const Sub_terrain& sub_terrain);

            const Terrain_definition& terrain_by_id(Terrain_id id) const;
            Terrain_id sub_terrain(Terrain_id id, std::size_t level) const;


        private:
            void define_sub_terrain(Terrain_id terrain, Terrain_id sub_id, std::size_t level);

            Terrain_definition terrains_[256];
            std::vector<Terrain_id> sub_terrains_;
        };
    }
}

#endif