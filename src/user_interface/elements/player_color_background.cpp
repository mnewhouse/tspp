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
#include "player_color_background.hpp"

#include "../gui_element.hpp"

namespace ts
{
    namespace gui
    {
        namespace impl
        {
            void add_base_color_vertices(std::vector<sf::Vertex>& vertices, const resources::Color_base& base);

            void add_extra_color_vertices(std::vector<sf::Vertex>& vertices, const resources::Color_extra& extra);
        }
    }
}

ts::gui::Player_color_background::Player_color_background(resources::Player_color color)
: player_color_(color)
{
    update_vertices();
}

void ts::gui::Player_color_background::draw(const gui::Element& element, sf::RenderTarget& render_target, sf::RenderStates render_states) const
{
    Vector2f elem_size = element.size();    

    render_states.transform.scale(elem_size.x, elem_size.y);

    render_target.draw(vertices_.data(), vertices_.size(), sf::Quads, render_states);

    const sf::Color shade_color(0, 0, 0, 200);
    const sf::Color transparent_color(0, 0, 0, 0);

    sf::Vertex shade[] =
    {
        sf::Vertex({ 0.0f, 0.0f }, shade_color),
        sf::Vertex({ 1.0f, 0.0f }, shade_color),
        sf::Vertex({ 1.0f, 0.1f }, transparent_color),
        sf::Vertex({ 0.0f, 0.1f }, transparent_color),

        sf::Vertex({ 0.0f, 1.0f }, shade_color),
        sf::Vertex({ 1.0f, 1.0f }, shade_color),
        sf::Vertex({ 1.0f, 0.9f }, transparent_color),
        sf::Vertex({ 0.0f, 0.9f }, transparent_color),

        sf::Vertex({ 0.0f, 0.0f }, shade_color),
        sf::Vertex({ 0.0f, 1.0f }, shade_color),
        sf::Vertex({ 0.1f, 1.0f }, transparent_color),
        sf::Vertex({ 0.1f, 0.0f }, transparent_color),

        sf::Vertex({ 1.0f, 0.0f }, shade_color),
        sf::Vertex({ 1.0f, 1.0f }, shade_color),
        sf::Vertex({ 0.9f, 1.0f }, transparent_color),
        sf::Vertex({ 0.9f, 0.0f }, transparent_color)
    };

    render_target.draw(shade, 16, sf::Quads, render_states);
}

void ts::gui::Player_color_background::set_color(resources::Player_color color)
{
    player_color_ = color;

    update_vertices();
}

void ts::gui::Player_color_background::update_vertices()
{
    vertices_.clear();

    impl::add_base_color_vertices(vertices_, player_color_.base_color);
    impl::add_extra_color_vertices(vertices_, player_color_.primary_extra);
    impl::add_extra_color_vertices(vertices_, player_color_.secondary_extra);
}

void ts::gui::Player_color_background::set_base_color(resources::Color_base base)
{
    player_color_.base_color = base;

    update_vertices();
}

void ts::gui::impl::add_base_color_vertices(std::vector<sf::Vertex>& vertices, const resources::Color_base& base)
{
    using resources::Color_base;

    if (base.type == Color_base::Plain)
    {
        vertices.push_back(sf::Vertex({ 0.0f, 0.0f }, base.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 0.0f }, base.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 1.0f }, base.primary_color));
        vertices.push_back(sf::Vertex({ 0.0f, 1.0f }, base.primary_color));
    }

    else if (base.type == Color_base::Vertical_gradient)
    {
        vertices.push_back(sf::Vertex({ 0.0f, 0.0f }, base.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 0.0f }, base.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 1.0f }, base.secondary_color));
        vertices.push_back(sf::Vertex({ 0.0f, 1.0f }, base.secondary_color));
    }

    else if (base.type == Color_base::Horizontal_gradient)
    {
        vertices.push_back(sf::Vertex({ 0.0f, 0.0f }, base.primary_color));
        vertices.push_back(sf::Vertex({ 0.0f, 1.0f }, base.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 1.0f }, base.secondary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 0.0f }, base.secondary_color));
    }
}

void ts::gui::impl::add_extra_color_vertices(std::vector<sf::Vertex>& vertices, const resources::Color_extra& extra)
{
    using resources::Color_extra;

    switch (extra.type)
    {
    case Color_extra::Front_half:
        vertices.push_back(sf::Vertex({ 0.0f, 0.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.0f, 0.5f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 0.5f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 0.0f }, extra.primary_color));
        break;

    case Color_extra::Rear_half:
        vertices.push_back(sf::Vertex({ 0.0f, 0.5f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.0f, 1.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 1.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 0.5f }, extra.primary_color));
        break;

    case Color_extra::Front_stripe:
        vertices.push_back(sf::Vertex({ 0.0f, 0.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.0f, 0.35f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 0.35f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 0.0f }, extra.primary_color));
        break;

    case Color_extra::Rear_stripe:
        vertices.push_back(sf::Vertex({ 0.0f, 1.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.0f, 0.65f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 0.65f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 1.0f }, extra.primary_color));
        break;

    case Color_extra::Left_half:
        vertices.push_back(sf::Vertex({ 0.0f, 0.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.0f, 1.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.5f, 1.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.5f, 0.0f }, extra.primary_color));
        break;

    case Color_extra::Right_half:
        vertices.push_back(sf::Vertex({ 0.5f, 0.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.5f, 1.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 1.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 0.0f }, extra.primary_color));
        break;

    case Color_extra::Double_stripe:
        vertices.push_back(sf::Vertex({ 0.30f, 0.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.30f, 1.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.45f, 1.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.45f, 0.0f }, extra.primary_color));

        vertices.push_back(sf::Vertex({ 0.55f, 0.0f }, extra.secondary_color));
        vertices.push_back(sf::Vertex({ 0.55f, 1.0f }, extra.secondary_color));
        vertices.push_back(sf::Vertex({ 0.70f, 1.0f }, extra.secondary_color));
        vertices.push_back(sf::Vertex({ 0.70f, 0.0f }, extra.secondary_color));
        break;

    case Color_extra::Vertical_stripe:
        vertices.push_back(sf::Vertex({ 0.35f, 0.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.35f, 1.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.65f, 1.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.65f, 0.0f }, extra.primary_color));
        break;

    case Color_extra::Gradient_stripe:
        vertices.push_back(sf::Vertex({ 0.35f, 0.0f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.35f, 1.0f }, extra.secondary_color));
        vertices.push_back(sf::Vertex({ 0.65f, 1.0f }, extra.secondary_color));
        vertices.push_back(sf::Vertex({ 0.65f, 0.0f }, extra.primary_color));
        break;

    case Color_extra::Reflected_gradient:
        vertices.push_back(sf::Vertex({ 0.0f, 0.0f }, sf::Color::Transparent));
        vertices.push_back(sf::Vertex({ 0.0f, 0.5f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 0.5f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 0.0f }, sf::Color::Transparent));

        vertices.push_back(sf::Vertex({ 0.0f, 0.5f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.0f, 1.0f }, sf::Color::Transparent));
        vertices.push_back(sf::Vertex({ 1.0f, 1.0f }, sf::Color::Transparent));
        vertices.push_back(sf::Vertex({ 1.0f, 0.5f }, extra.primary_color));
        break;

    case Color_extra::Horizontal_stripe:
        vertices.push_back(sf::Vertex({ 0.0f, 0.35f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 0.0f, 0.65f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 0.65f }, extra.primary_color));
        vertices.push_back(sf::Vertex({ 1.0f, 0.35f }, extra.primary_color));
        break;

    default:
        break;
    }
}