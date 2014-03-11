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

#ifndef FONTS_HPP
#define FONTS_HPP

#include <SFML/Graphics/Font.hpp>

#include "font_base.hpp"

#include "core/utility.hpp"

namespace ts
{

namespace fonts
{

typedef sf::Font Font;

namespace impl
{
    typedef type_id_type font_id;

    const Font& load_font_from_memory(font_id id, const void* ptr, std::size_t buffer_size);
    const Font& get_font_by_id(font_id id);
    void unload_all();
}

struct Invalid_font
    : public std::runtime_error
{
    Invalid_font();
};

struct Font_not_found
    : public std::runtime_error
{
    Font_not_found();
};

template <typename T>
typename std::enable_if<std::is_base_of<Font_base, T>::value>::type load_font(const T& font)
{
    auto id = type_id<T>();
    impl::load_font_from_memory(id, font.data(), font.size());
}

template <typename T>
const Font& get_font_by_type()
{
    auto id = type_id<T>();
    return impl::get_font_by_id(id);
}

}

}
#endif