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

#ifndef GAME_CAR_IMAGE_GENERATOR_HPP
#define GAME_CAR_IMAGE_GENERATOR_HPP

#include <SFML/Graphics.hpp>

namespace ts
{
    namespace resources
    {
        struct Car_definition;
        struct Player_color;
    }

    namespace game
    {
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
            const sf::Image& get_image_by_filename(const utf8_string& file_name) const;
            const sf::Image& load_image_from_file(const utf8_string& file_name) const;

            mutable std::map<utf8_string, sf::Image> image_map_;
            mutable std::vector<char> file_buffer_;
        };
    }
}

#endif