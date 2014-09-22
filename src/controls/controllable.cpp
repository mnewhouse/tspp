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
#include "controllable.hpp"

ts::controls::Controllable::Controllable()
    : control_state_(0)
{
}

void ts::controls::Controllable::set_control_state(Control control, bool state)
{
    if (state) {
        auto new_state = control_state_ | static_cast<unsigned>(control);
        if (new_state != control_state_) {
            control_state_ = new_state;

            // Event
        }
    }

    else {
        auto new_state = control_state_ & ~static_cast<unsigned>(control);
        if (new_state != control_state_) {
            control_state_ = new_state;

            // Event
        }
    }
}

bool ts::controls::Controllable::control_state(Control control) const
{
    return (control_state_ & static_cast<unsigned>(control)) != 0;
}

void ts::controls::Controllable::update_control_state(unsigned new_state)
{
    control_state_ = new_state;
}

unsigned ts::controls::Controllable::control_state() const
{
    return control_state_;
}