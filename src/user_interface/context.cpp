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

ts::gui::Context::Context()
    : mouse_state_(), 
      next_id_(0), 
      hot_item_(0),
      active_item_(0)
{
}

ts::gui::element_id ts::gui::Context::allocate_base_id()
{
    return next_id_ += 0xFFFF;
}

void ts::gui::Context::reset()
{
    hot_item_ = 0;
    mouse_state_.scroll_offset = 0;

    if (~mouse_state_.button_state & mouse::Button::Left) {
        active_item_ = 0;
    }
}

const ts::gui::mouse::State& ts::gui::Context::mouse_state() const
{
    return mouse_state_;
}

auto ts::gui::Context::hot_item() const -> element_id
{
    return hot_item_;
}

auto ts::gui::Context::active_item() const -> element_id
{
    return active_item_;
}

void ts::gui::Context::set_mouse_state(const mouse::State& new_state)
{
    mouse_state_ = new_state;
}

void ts::gui::Context::set_hot_item(element_id id)
{
    hot_item_ = id;
}

void ts::gui::Context::set_active_item(element_id id)
{
    active_item_ = id;
}