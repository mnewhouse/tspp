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

#include "context.hpp"

#include "gui_element.hpp"

ts::gui::Context::Context(Vector2u screen_size)
    : mouse_state_(), screen_size_(screen_size)
{
}

const ts::gui::mouse::State& ts::gui::Context::mouse_state() const
{
    return mouse_state_;
}

void ts::gui::Context::set_mouse_state(const mouse::State& new_state)
{
    mouse_state_ = new_state;
}

const ts::Vector2u& ts::gui::Context::screen_size() const
{
    return screen_size_;
}
