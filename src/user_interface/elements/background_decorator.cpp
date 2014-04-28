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

#include "background_decorator.hpp"

#include "../gui_element.hpp"

ts::gui::elements::Background_color::Background_color(sf::Color color)
    : color_(color)
{
}

void ts::gui::elements::Background_color::set_color(sf::Color color)
{
    color_ = color;
}

sf::Color ts::gui::elements::Background_color::color() const
{
    return color_;
}

void ts::gui::elements::Background_color::draw(const Element& element, graphics::Render_target& render_target,
                    graphics::Render_states render_states) const
{
    auto size = element.size();

    sf::Vertex vertices[4];
    vertices[0].position = sf::Vector2f(0.0, 0.0);
    vertices[1].position = sf::Vector2f(0.0, size.y);
    vertices[2].position = sf::Vector2f(size.x, size.y);
    vertices[3].position = sf::Vector2f(size.x, 0.0);

    for (auto& vertex : vertices) vertex.color = color_;

    render_target.draw(vertices, 4, sf::Quads, render_states);
}