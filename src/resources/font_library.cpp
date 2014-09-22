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

#include "stdinc.hpp"
#include "font_library.hpp"

#include "fonts/sans.hpp"
#include "fonts/monospace.hpp"

const ts::utf8_string ts::resources::fonts::Sans = "sans";
const ts::utf8_string ts::resources::fonts::Monospace = "monospace";

ts::resources::Font_library::Font_library()
{
    load_builtin_fonts();
}

ts::resources::Font_face ts::resources::Font_library::load_font_from_memory(utf8_string font_name, const void* data, std::size_t data_size)
{
    Font_face font_face(data, data_size);
    loaded_fonts_.emplace(std::move(font_name), font_face);
    return font_face;
}

ts::resources::Font_face ts::resources::Font_library::font_by_name(const utf8_string& font_name) const
{
    auto it = loaded_fonts_.find(font_name);
    if (it != loaded_fonts_.end()) 
    {
        return it->second;
    }

    return Font_face();
}

void ts::resources::Font_library::load_builtin_fonts()
{
    ts::fonts::Sans sans;
    load_font_from_memory(fonts::Sans, sans.data(), sans.size());

    ts::fonts::Monospace monospace;
    load_font_from_memory(fonts::Monospace, monospace.data(), monospace.size());
}