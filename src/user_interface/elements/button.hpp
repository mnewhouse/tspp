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

#ifndef GUI_ELEMENTS_BUTTON_HPP
#define GUI_ELEMENTS_BUTTON_HPP

#include "text_item.hpp"

namespace ts
{
    namespace gui
    {
        namespace elements
        {
            class Button
                : public Text_item
            {
            public:
                Button(Vector2i area_size, const std::string& text, const sf::Font& font, std::uint32_t character_size)
                    : Text_item(text, font, character_size)
                {
                    set_size(area_size);

                    base_color_ = hover_color_ = Text_item::color();
                } 

                Button(const std::string& text, const sf::Font& font, std::uint32_t character_size)
                    : Text_item(text, font, character_size)
                {
                    base_color_ = hover_color_ = Text_item::color();
                }        

                void set_base_color(sf::Color color)
                {
                    if (!hover()) set_color(color);

                    base_color_ = color;
                }
                void set_hover_color(sf::Color hover_color)
                {
                    if (hover()) set_color(hover_color);

                    hover_color_ = hover_color;
                }

                virtual void on_mouse_enter() override
                {
                    Text_item::set_color(hover_color_);
                }

                virtual void on_mouse_leave() override
                {
                    Text_item::set_color(base_color_);
                }
                    
            private:
                sf::Color base_color_;
                sf::Color hover_color_;
            };
        }
    }
}

#endif