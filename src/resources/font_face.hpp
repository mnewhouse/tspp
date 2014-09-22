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

#ifndef GRAPHICS_FONT_FACE_HPP
#define GRAPHICS_FONT_FACE_HPP

#include <SFML/Graphics.hpp>

namespace ts
{
    namespace resources
    {
        struct Glyph
        {
            Vector2i advance_offset;
            Rect<std::int32_t> bounds;
            Rect<std::int32_t> texture_rect;
        };

        namespace impl
        {
            struct Font_face;
        }

        class Font_face
        {
        public:
            Font_face() = default;
            Font_face(const void* data, std::size_t data_size);    

            const Glyph& glyph(utf8::uint32_t code_point, std::uint32_t character_size) const;
            const sf::Texture* texture(std::uint32_t character_size) const;

            Vector2i kerning(utf8::uint32_t first, utf8::uint32_t second, std::uint32_t character_size) const;
            std::uint32_t default_line_height(std::uint32_t character_size) const;

            operator bool() const;

        private:
            std::shared_ptr<impl::Font_face> impl_;
        };
    }
}

#endif