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
            Colored_text(utf8_string text = utf8_string(), sf::Color color = sf::Color::Transparent);

            utf8_string text;
            sf::Color color;
        };

        struct Composite_text_iterator_value_type
        {
            sf::Color color;
            std::uint32_t code_point;
        };

        struct Composite_text_iterator
            : public std::iterator<std::forward_iterator_tag, Composite_text_iterator_value_type>
        {
        public:
            Composite_text_iterator& operator++();
            Composite_text_iterator operator++(int);

            value_type operator*() const;

            bool operator==(const Composite_text_iterator& other) const;
            bool operator!=(const Composite_text_iterator& other) const;

        private:
            using component_iterator = std::vector<Colored_text>::const_iterator;

            friend class Composite_text;
            Composite_text_iterator(component_iterator component_it, component_iterator component_end);

            component_iterator component_it_;
            component_iterator component_end_;
            utf8_string::iterator text_it_;
        };

        class Composite_text
        {
        public:
            Composite_text() = default;
            Composite_text(Colored_text text);
            Composite_text(utf8_string text, sf::Color color = sf::Color::Transparent);

            void append(Colored_text text);
            void append(const Composite_text& composite_text);
            void append(utf8_string text, sf::Color color = sf::Color::Transparent);

            Composite_text& operator+=(const Colored_text& text);
            Composite_text& operator+=(const Composite_text& text);

            const std::vector<Colored_text>& components() const;
            const utf8_string& text() const;

            using iterator = Composite_text_iterator;
            iterator begin() const;
            iterator end() const;

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