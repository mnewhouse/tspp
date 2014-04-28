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

#ifndef GUI_ELEMENTS_TEXT_ITEM_HPP
#define GUI_ELEMENTS_TEXT_ITEM_HPP

#include "button.hpp"

#include "graphics/renderer.hpp"

#include <string>

namespace ts
{
    namespace gui
    {
        namespace elements
        {
            struct Text_item
                : public Element
            {
            public:
                Text_item(const std::string& text, const sf::Font& font, unsigned character_size)
                    : text_display_(text, font, character_size),
                      center_horizontal_(false)
                {
                }

                void set_character_size(unsigned size)
                {
                    text_display_.setCharacterSize(size);

                    center_();
                }
  

                void set_text(const std::string& text)
                {
                    text_display_.setString(text);

                    center_();
                }

                void set_color(sf::Color color)
                {
                    text_display_.setColor(color);
                }

                sf::Color color() const
                {
                    return text_display_.getColor();
                }

                virtual void draw(graphics::Render_target& render_target, graphics::Render_states render_states) const override
                {
                    render_target.draw(text_display_, render_states);
                }

                void center_horizontal()
                {
                    center_horizontal_ = true;

                    center_();
                }

            private:
                void center_()
                {
                    if (center_horizontal_)
                    {
                        auto bounds = text_display_.getLocalBounds();

                        set_offset((size().x / 2) - (bounds.width / 2), offset().y);
                    }
                }


                bool center_horizontal_;

                sf::Text text_display_;
            };
        }
    }
}






#endif