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


#ifndef GUI_ELEMENTS_VERTEX_ARRAY_HPP
#define GUI_ELEMENTS_VERTEX_ARRAY_HPP

#include "../gui_element.hpp"
#include "button.hpp"

namespace ts
{
    namespace gui
    {
        namespace elements
        {
            class Vertex_array
                : public Element
            {
            public:
                Vertex_array(const sf::VertexArray& vertex_array)
                    : vertex_array_(vertex_array)
                {}

                virtual void draw(graphics::Render_target& render_target, graphics::Render_states render_states) const override
                {
                    render_target.draw(vertex_array_, render_states);
                }

            private:
                sf::VertexArray vertex_array_;
            };

            template <typename RegionType>
            class Vertex_button
                : public Button<RegionType>
            {
            public:
                Vertex_button(const RegionType& region, const sf::VertexArray& vertex_array, 
                              const sf::VertexArray& hover_array = vertex_array)
                    : Button<RegionType>(region),
                      vertex_array_(vertex_array),
                      hover_vertex_array_(hover_array)
                {}

                virtual void draw(graphics::Render_target& render_target, graphics::Render_states render_states) const override
                {
                    const auto& drawable = Element::hover() ? hover_vertex_array_ : vertex_array_;
                    render_target.draw(drawable, render_states);
                }

            private:
                sf::VertexArray vertex_array_;
                sf::VertexArray hover_vertex_array_;
            };

            using Rectangular_vertex_button = Vertex_button<Rectangle_region>;
        }
    }
}

#endif