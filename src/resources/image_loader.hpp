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

#ifndef RESOURCES_IMAGE_LOADER_HPP
#define RESOURCES_IMAGE_LOADER_HPP

#include <SFML/Graphics/Image.hpp>

namespace ts
{
    namespace resources
    {
        struct Image_load_error
            : std::logic_error
        {
            Image_load_error(utf8_string file_path);

            const utf8_string& file_path() const;

        private:
            utf8_string file_path_;
        };

        class Image_loader
        {
        public:            
            const sf::Image& load_from_file(const utf8_string& file_name);
            const sf::Image* load_from_file(const utf8_string& file_name, std::nothrow_t);

        private:            
            const sf::Image* load_from_file_impl(const utf8_string& file_name);

            std::map<utf8_string, sf::Image> image_map_;
            std::vector<char> file_buffer_;
        };
    }
}

#endif