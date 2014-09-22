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

#ifndef RESOURCES_FONT_LIBRARY_HPP
#define RESOURCES_FONT_LIBRARY_HPP

#include "font_face.hpp"

namespace ts
{
    namespace resources
    {
        namespace fonts
        {
            extern const utf8_string Sans;
            extern const utf8_string Monospace;            
        }        

        class Font_library
        {
        public:
            Font_library();

            Font_face load_font_from_memory(utf8_string font_name, const void* data, std::size_t data_size);
            Font_face font_by_name(const utf8_string& font_name) const;            

        private:
            void load_builtin_fonts();

            std::unordered_map<utf8_string, Font_face> loaded_fonts_;
        };
    }
}

#endif