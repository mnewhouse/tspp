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

#ifndef RESOURCES_TERRAIN_DEFINITION_HPP
#define RESOURCES_TERRAIN_DEFINITION_HPP

#include <cstdint>
#include <string>
#include <istream>

#include "wall_definition.hpp"

namespace ts
{
    namespace resources
    {
        using Terrain_id = std::uint8_t;

        struct Terrain_definition
        {
            explicit Terrain_definition();

            Terrain_id id;

            double acceleration;
            double viscosity;
            double steering;
            double grip;
            double anti_slide;
            double braking;

            bool is_wall;
            Wall_definition wall_definition;
            
            std::uint32_t color[3];
        };

        std::istream& operator>>(std::istream& stream, Terrain_definition& terrain_def);
    }
}

#endif