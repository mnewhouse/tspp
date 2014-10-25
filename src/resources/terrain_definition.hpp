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

#ifndef RESOURCES_TERRAIN_DEFINITION_HPP
#define RESOURCES_TERRAIN_DEFINITION_HPP

#include "wall_definition.hpp"

namespace ts
{
    namespace resources
    {
        using Terrain_id = std::uint8_t;

        struct Terrain_color
        {
            std::uint8_t red;
            std::uint8_t green;
            std::uint8_t blue;
        };

        struct Terrain_definition
        {
            explicit Terrain_definition();

            Terrain_id id;

            double acceleration;
            double roughness;
            double steering;
            double traction;
            double antislide;
            double braking;

            bool tire_mark;
            bool skid_mark;

            bool is_wall;
            Wall_definition wall_definition;
            
            Terrain_color color;
        };

        struct Sub_terrain
        {
            Terrain_id terrain_id;
            Terrain_id component_id;
            std::int32_t level_start;
            std::int32_t level_count;
        };

        std::istream& operator>>(std::istream& stream, Terrain_definition& terrain_def);
        std::istream& operator>>(std::istream& stream, Sub_terrain& sub_terrain);

        bool operator==(const Terrain_definition& first, const Terrain_definition& second);
        bool operator!=(const Terrain_definition& first, const Terrain_definition& second);
    }
}

#endif