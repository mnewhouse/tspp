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

#ifndef GRAPHICS_TEXT_HPP
#define GRAPHICS_TEXT_HPP

#include "colored_text.hpp"
#include "geometry.hpp"

#include "resources/font_face.hpp"

namespace ts
{
    namespace graphics
    {
        using resources::Font_face;

        class Text
        {
        public:
            Text() = default;
            Text(Composite_text text, Font_face font, std::uint32_t character_size);
            Text(utf8_string text, Font_face font, std::uint32_t character_size);

            void set_text(utf8_string text);
            void set_text(Composite_text text);

            void set_color(sf::Color color);
            void set_character_size(std::uint32_t character_size);
            void set_font(Font_face font);

            void set_line_height(std::uint32_t line_height);
            void set_word_wrap(std::uint32_t max_width);

            void set_position(Vector2<double> position);
            Vector2<double> position() const;

            const Geometry& geometry() const;
            const Double_rect& bounds() const;

            Vector2<double> character_position(std::size_t index) const;
            double base_line(std::size_t character_index) const;

            std::uint32_t character_size() const;

            double line_height() const;
            const utf8_string& text() const;
            const Composite_text& formatted_text() const;
            sf::Color color() const;

        private:
            void update_geometry() const;            

            Composite_text text_;
            Font_face font_;            
            sf::Color color_;
            std::uint32_t character_size_ = 0;
            std::uint32_t word_wrap_width_ = 0;
            std::uint32_t line_height_ = 0;

            Vector2<double> position_;

            mutable struct Text_geometry
            {
                bool dirty = true;
                Geometry geometry;
                Rect<double> bounds;
                std::vector<std::size_t> line_breaks;
                std::vector<double> character_offsets;
            } geometry_;           
        };
    }
}



#endif