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

#ifndef EVENT_DISPATCHER_HPP
#define EVENT_DISPATCHER_HPP

#include "event_handler.hpp"

namespace ts
{
    namespace game
    {
        class Event_dispatcher
            : public core::Listener_host<Event_handler>
        {
        public:
            Generic_scope_exit acquire_event_hog(Event_handler* hogger);
            void release_event_hog(Event_handler* hogger);

            void handle_event(const sf::Event& event);
            bool hogged() const;

        private:
            Event_handler* event_hogger_ = nullptr;
        };
    }
}

#endif