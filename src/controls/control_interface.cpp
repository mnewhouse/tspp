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

#include "control_interface.hpp"
#include "control_center.hpp"

ts::controls::Control_interface::Control_interface(const Key_mapping& key_mapping)
: key_mapping_(key_mapping)
{
}

void ts::controls::Control_interface::forward_input(const sf::Event& event, const Control_center& control_center) const
{
    auto forward_key_event = [&](Key key, bool state)
    {
        auto range = key_mapping_.get_all_binds_by_key(key);
        for (auto it = range.first; it != range.second; ++it)
        {
            control_center.set_control_state(it->first.slot, it->second, state);
        }       
    };

    if (event.type == sf::Event::KeyPressed)
    {
        forward_key_event(event.key.code, true);
    }

    else if (event.type == sf::Event::KeyReleased)
    {
        forward_key_event(event.key.code, false);
    }
}