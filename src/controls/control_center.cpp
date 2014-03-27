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

#include "control_center.hpp"
#include "controllable.hpp"

void ts::controls::Control_center::update_control_state(Slot slot, std::uint32_t new_state)
{
    auto range = control_mapping_.equal_range(slot);
    for (auto it = range.first; it != range.second; ++it)
    {
        Controllable* controllable = it->second;
        controllable->update_control_state(new_state & ~globally_disabled_controls_);
    }
}

void ts::controls::Control_center::set_control_state(Slot slot, Control control, bool state)
{
    if ((globally_disabled_controls_ & std::uint32_t(control)) != 0) return;

    auto range = control_mapping_.equal_range(slot);
    for (auto it = range.first; it != range.second; ++it)
    {
        Controllable* controllable = it->second;
        controllable->set_control_state(control, state);
    }
}

void ts::controls::Control_center::assume_control(Slot slot, Controllable* controllable)
{
    control_mapping_.insert(std::make_pair(slot, controllable));
}

void ts::controls::Control_center::toggle_global_control(Control control, bool enable)
{
    globally_disabled_controls_ |= static_cast<std::uint32_t>(control);

    if (enable)
    {
        globally_disabled_controls_ ^= static_cast<std::uint32_t>(control);
    }
}