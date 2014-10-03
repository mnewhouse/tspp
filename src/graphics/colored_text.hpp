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

#ifndef GRAPHICS_COLORIZED_TEXT_HPP
#define GRAPHICS_COLORIZED_TEXT_HPP

#include <SFML/Graphics/Color.hpp>

namespace ts
{
    namespace graphics
    {
        struct Colored_text
        {
            Colored_text(utf8_string text, sf::Color color = sf::Color::Transparent);

            utf8_string text;
            sf::Color color;
        };

        class Composite_text
        {
        public:
            Composite_text(Colored_text text);
            Composite_text(utf8_string text, sf::Color color = sf::Color::Transparent);

            void append(Colored_text text);
            void append(const Composite_text& composite_text);

            Composite_text& operator+=(const Colored_text& text);
            Composite_text& operator+=(const Composite_text& text);

            const std::vector<Colored_text>& components() const;
            const utf8_string& text() const;

        private:
            std::vector<Colored_text> components_;
            utf8_string text_;
        };

        Composite_text operator+(Composite_text lhs, const Composite_text& rhs);
        Composite_text operator+(const Colored_text& lhs, const Colored_text& rhs);
        Composite_text operator+(const Colored_text& lhs, const Composite_text& rhs);
        Composite_text operator+(Composite_text lhs, const Colored_text& rhs);
    }
}
#endif