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

#ifndef GUI_MOUSE_HPP
#define GUI_MOUSE_HPP

#include "core/vector2.hpp"

namespace ts
{

    namespace gui
    {

        namespace mouse
        {

            struct Button
            {
                static const unsigned Left = 1;
                static const unsigned Middle = 2;
                static const unsigned Right = 4;
            };

            struct State
            {
                State()
                : position(), scroll_offset(0), button_state(0)
                {}

                Vector2<int> position;
                int scroll_offset;
                unsigned button_state;
            };

        }

    }

}

#endif