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

#ifndef RESOURCES_CAR_IMAGE_GENERATOR_HPP
#define RESOURCES_CAR_IMAGE_GENERATOR_HPP

#include "image_loader.hpp"

namespace ts
{
    namespace resources
    {
        struct Car_definition;
        struct Player_color;

        class Car_image_generator
        {
        public:
            enum Frame_mode
            {
                Single_frame,
                All_frames
            };

            sf::Image operator()(const resources::Car_definition& car_definition, const resources::Player_color& color_scheme, Frame_mode = All_frames) const;

        private:
            mutable resources::Image_loader image_loader_;
        };
    }
}

#endif