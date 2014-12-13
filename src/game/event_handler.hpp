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

#ifndef EVENT_HANDLER_HPP
#define EVENT_HANDLER_HPP

#include <SFML/Window/Event.hpp>

namespace ts
{
    namespace game
    {
        struct Event_handler
        {
            virtual void handle_event(const sf::Event& event) {}
        };

        using Event_handler_host = core::Listener_host<Event_handler>;
        using Scoped_event_handler = core::Scoped_listener<Event_handler>;
    }
}

#endif