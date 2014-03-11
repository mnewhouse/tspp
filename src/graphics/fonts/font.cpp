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

#include "font.hpp"

#include <unordered_map>

namespace {
    std::unordered_map<ts::fonts::impl::font_id, sf::Font> loaded_fonts;
}

const sf::Font& ts::fonts::impl::load_font_from_memory
    (font_id id, const void* buffer, std::size_t buffer_size)
{
    auto& font = loaded_fonts[id];
    if (!font.loadFromMemory(buffer, buffer_size)) {
        throw Invalid_font();
    }

    return font;
}

const sf::Font& ts::fonts::impl::get_font_by_id(font_id id)
{
    auto it = loaded_fonts.find(id);
    if (it == loaded_fonts.end()) {
        throw Font_not_found();
    }

    return it->second;
}

void ts::fonts::impl::unload_all()
{
    loaded_fonts.clear();
}

ts::fonts::Font_not_found::Font_not_found()
    : std::runtime_error("font not found")
{}

ts::fonts::Invalid_font::Invalid_font()
    : std::runtime_error("invalid font supplied")
{}