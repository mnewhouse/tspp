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

#ifndef RESOURCES_PLAYER_COLOR_HPP
#define RESOURCES_PLAYER_COLOR_HPP

#include <SFML/Graphics/Color.hpp>

namespace ts
{
    namespace resources
    {
        struct Color_base
        {
            enum Type
            {
                Plain = 0,
                Vertical_gradient,
                Horizontal_gradient,

                Max_value // for internal use only
            };

            Type type;
            sf::Color primary_color;
            sf::Color secondary_color;
        };


        struct Color_extra
        {
            enum Type
            {
                None = 0,

                Vertical_stripe,
                Gradient_stripe,
                Double_stripe,

                Reflected_gradient,

                Horizontal_stripe,
                Front_stripe,
                Rear_stripe,

                Front_half,
                Rear_half,

                Left_half,
                Right_half,

                Max_value
            };

            Type type = None;

            sf::Color primary_color;
            sf::Color secondary_color;
        };

        struct Player_color
        {
            Player_color(sf::Color color = sf::Color::Black);

            Color_base base_color;
            Color_extra primary_extra;
            Color_extra secondary_extra;
        };

        utf8_string to_string(Color_base::Type type);        
        utf8_string to_string(Color_extra::Type type);
    }
}

#endif