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

#ifndef RESOURCES_CAR_DEFINITION_HPP
#define RESOURCES_CAR_DEFINITION_HPP

#include <istream>
#include <string>
#include <vector>
#include <memory>

#include "image_type.hpp"
#include "wall_definition.hpp"

#include "world/handling.hpp"

#include "core/rect.hpp"

namespace ts
{

    namespace resources
    {
        class Pattern;
        
        struct Car_definition
        {
            std::string file_path;

            std::string car_name;
            Int_rect image_rect;
            float image_scale;
            std::string image_file;
            Image_type image_type;

            std::shared_ptr<Pattern> pattern;
            Wall_definition wall_definition;

            world::Handling_properties handling;
        };

        std::istream& operator>>(std::istream& stream, Car_definition& car_definition);

        std::vector<Car_definition> load_car_definitions(std::istream& stream);

    }

}

#endif