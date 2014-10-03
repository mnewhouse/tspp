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
#include "colored_text.hpp"

ts::graphics::Colored_text::Colored_text(utf8_string text, sf::Color color)
: text(std::move(text)),
  color(color)
{}

ts::graphics::Composite_text::Composite_text(Colored_text text)
{
    append(text);
}

ts::graphics::Composite_text::Composite_text(utf8_string text, sf::Color color)
{
    append(Colored_text(std::move(text), color));
}

void ts::graphics::Composite_text::append(Colored_text text)
{
    text_ += text.text;
    components_.push_back(std::move(text));    
}

void ts::graphics::Composite_text::append(const Composite_text& Composite_text)
{
    for (auto& component : Composite_text.components())
    {
        append(component);
    }
}

ts::graphics::Composite_text& ts::graphics::Composite_text::operator+=(const Colored_text& text)
{
    append(text);
    return *this;
}

ts::graphics::Composite_text& ts::graphics::Composite_text::operator+=(const Composite_text& Composite_text)
{
    append(Composite_text);
    return *this;
}

const std::vector<ts::graphics::Colored_text>& ts::graphics::Composite_text::components() const
{
    return components_;
}

const ts::utf8_string& ts::graphics::Composite_text::text() const
{
    return text_;
}

ts::graphics::Composite_text ts::graphics::operator+(Composite_text lhs, const Composite_text& rhs)
{
    lhs += rhs;
    return lhs;
}

ts::graphics::Composite_text ts::graphics::operator+(Composite_text lhs, const Colored_text& rhs)
{
    lhs += rhs;
    return lhs;
}

ts::graphics::Composite_text ts::graphics::operator+(const Colored_text& lhs, const Colored_text& rhs)
{
    Composite_text result(lhs);
    result += rhs;
    return result;
}

ts::graphics::Composite_text ts::graphics::operator+(const Colored_text& lhs, const Composite_text& rhs)
{
    Composite_text result(lhs);
    result += rhs;
    return result;
}