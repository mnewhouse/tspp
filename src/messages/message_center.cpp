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
#include "message_center.hpp"
#include "message_listener.hpp"
#include "message_dispatcher.hpp"

void ts::messages::Message_center::handle_message(const Message& message)
{
    for (auto listener : message_listeners_)
    {
        listener->handle_message(message);
    }
}

void ts::messages::Message_center::dispatch_message(const Message& message)
{
    for (auto dispatcher : message_dispatchers_)
    {
        dispatcher->dispatch_message(message);
    }

    handle_message(message);
}

void ts::messages::Message_center::add_message_listener(listener_type* message_listener)
{
    message_listeners_.push_back(message_listener);
}

void ts::messages::Message_center::remove_message_listener(listener_type* message_listener)
{
    message_listeners_.erase(std::remove(message_listeners_.begin(), message_listeners_.end(), message_listener), message_listeners_.end());
}



void ts::messages::Message_center::add_message_dispatcher(dispatcher_type* message_dispatcher)
{
    message_dispatchers_.push_back(message_dispatcher);
}

void ts::messages::Message_center::remove_message_dispatcher(dispatcher_type* message_dispatcher)
{
    message_dispatchers_.erase(std::remove(message_dispatchers_.begin(), message_dispatchers_.end(), message_dispatcher), message_dispatchers_.end());
}