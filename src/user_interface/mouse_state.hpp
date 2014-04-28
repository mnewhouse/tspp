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

#ifndef GUI_MOUSE_HPP
#define GUI_MOUSE_HPP

#include "core/vector2.hpp"

namespace ts
{
    namespace gui
    {
        namespace mouse
        {
            enum class Button
                : std::uint32_t
            {
                None = 0,
                Left = 1,
                Right = 2,
                Middle = 4
            };

            struct State
            {
                State()
                    : position(), scroll_offset(0), button_state(0)
                {}

                Vector2i position;
                int scroll_offset = 0;
                unsigned button_state = 0;
            };
        }
    }
}

#endif