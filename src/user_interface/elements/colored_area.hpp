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

#ifndef GUI_ELEMENTS_COLORED_AREA_HPP
#define GUI_ELEMENTS_COLORED_AREA_HPP

#include "../gui_element.hpp"

#include "core/vector2.hpp"

namespace ts
{
    namespace gui
    {
        namespace elements
        {
            class Colored_area
                : public Element
            {
            public:
                Colored_area(Vector2i size, sf::Color color)
                {
                    set_size(size);

                    rect_area_.setFillColor(color);
                }

                virtual void draw(graphics::Render_target& render_target, graphics::Render_states render_states) const override
                {
                    render_target.draw(rect_area_, render_states);
                }

                virtual void on_resize(Vector2i old_size, Vector2i new_size)
                {
                    rect_area_.setSize(sf::Vector2f(new_size.x, new_size.y));
                }

            private:
                sf::RectangleShape rect_area_;
            };
        }
    }
}

#endif