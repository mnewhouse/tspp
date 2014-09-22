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

#ifndef GUI_EDIT_BOX_HPP
#define GUI_EDIT_BOX_HPP

#include "text_element.hpp"

namespace ts
{
    namespace gui
    {
        class Edit_box
            : public Text_element
        {
        public:
            Edit_box(Text_style style);

            virtual void draw(sf::RenderTarget& render_target, sf::RenderStates render_states) const override;

            void reset();
            void set_max_length(std::size_t max_length);


        private:
            void register_event_listeners();

            std::size_t caret_index_ = 0;
            std::size_t max_length_ = 0;
            bool active_ = false;

            mutable struct cursor_data
            {
                bool show = true;
                sf::Clock clock;
            } cursor_;
        };
    }
}

#endif