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
#include "control_event_translator.hpp"

ts::controls::Event_translator::Event_translator(const Key_mapping& key_mapping)
: key_mapping_(key_mapping)
{
}

void ts::controls::Event_translator::enqueue_event(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
    {
        Control_event control_event;
        control_event.state = (event.type == sf::Event::KeyPressed);

        auto binds = key_mapping_.get_all_binds_by_key(event.key.code);
        for (const auto& bind : binds)
        {
            control_event.slot = bind.first.slot;
            control_event.control = bind.second;
            enqueue_event(control_event);
        }
    }
}

void ts::controls::Event_translator::enqueue_event(const Control_event& event)
{
    event_queue_.push_back(event);
}

bool ts::controls::Event_translator::poll_event(Control_event& event)
{
    if (event_queue_.empty())
    {
        return false;
    }

    event = event_queue_.front();
    event_queue_.pop_front();
    return true;
}