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

#ifndef RESOURCES_CAR_DEFINITION_HPP
#define RESOURCES_CAR_DEFINITION_HPP

#include "image_type.hpp"
#include "wall_definition.hpp"

#include "core/rect.hpp"

namespace ts
{
    namespace world
    {
        struct Handling_properties;
        class Collision_bitmap;
    }

    namespace resources
    {
        class Pattern;
        
        struct Car_definition
        {
            utf8_string file_path;

            utf8_string car_name;

            utf8_string pattern_file;
            Int_rect pattern_rect;
            std::size_t pattern_rotation_count = 64;

            std::shared_ptr<world::Collision_bitmap> collision_bitmap;
            std::shared_ptr<world::Handling_properties> handling;

            Int_rect image_rect;
            float image_scale;
            utf8_string image_file;
            Image_type image_type;

            Wall_definition wall_definition;

            std::vector<Vector2<double>> tire_positions;

            utf8_string engine_sample;
        };

        Car_definition load_car_definition(std::istream& stream, const utf8_string& directory);
        std::vector<Car_definition> load_car_definitions(std::istream& stream, const utf8_string& directory);

    }

}

#endif