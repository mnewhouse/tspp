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
#include "action_messages.hpp"

#include "world/world.hpp"

ts::action::Message ts::action::make_game_state_message(const world::World* world)
{
    return Message();
}

ts::action::Message ts::action::make_control_event_message(const controls::Controllable* controllable, const std::uint32_t stage_time)
{
    Message result;
    result << static_cast<std::uint8_t>(0x80);
    result << static_cast<std::uint16_t>(controllable->controllable_id());
    result << static_cast<std::uint8_t>(controllable->control_state());
    result << stage_time;
    return result;
}

bool ts::action::is_control_event_message(const Message& message)
{
    return !message.empty() && (*message.data() & 0x80);
}