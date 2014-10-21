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
#include "control_center.hpp"
#include "control_event.hpp"
#include "controllable.hpp"


void ts::controls::Control_center::handle_control_event(const Control_event& event) const
{
    if ((static_cast<std::uint32_t>(event.control) & globally_disabled_controls_) == 0)
    {
        // If control is disabled, ignore event
        auto range = control_mapping_.equal_range(event.slot);
        for (auto it = range.first; it != range.second; ++it)
        {
            Controllable* controllable = it->second;
            controllable->set_control_state(event.control, event.state);
        }
    }
}

ts::controls::Slot ts::controls::Control_center::get_controllable_slot(const Controllable* controllable) const
{
    for (auto& mapping : control_mapping_)
    {
        if (mapping.second == controllable)
        {
            return mapping.first;
        }
    }

    return invalid_slot;
}

const std::vector<const ts::controls::Controllable*>& ts::controls::Control_center::get_controllables_by_slot(Slot slot) const
{
    controllable_list_.clear();

    auto range = control_mapping_.equal_range(slot);
    for (; range.first != range.second; ++range.first)
    {
        controllable_list_.push_back(range.first->second);
    }

    return controllable_list_;
}

bool ts::controls::Control_center::is_controlled(const Controllable* controllable) const
{
    return get_controllable_slot(controllable) != invalid_slot;
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