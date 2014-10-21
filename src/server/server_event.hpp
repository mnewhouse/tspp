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

#ifndef SERVER_EVENT_HPP
#define SERVER_EVENT_HPP

#include "cup/cup_metadata.hpp"

#include "generic_client.hpp"

namespace ts
{
    namespace server
    {
        struct State_change
        {
            cup::Cup_state old_state;
            cup::Cup_state new_state;
        };

        struct Event
        {
            enum Type
            {
                State_change,
                Initialize,
                Load,

                Client_connect,
                Client_disconnect                
            };

            Type type;
            State_change state_change;
            Generic_client client;
        };

        class Event_queue
        {
        public:
            void add_event(const Event& event);
            bool poll_event(Event& event);

        private:
            std::deque<Event> event_queue_;
        };

        Event make_state_change_event(cup::Cup_state old_state, cup::Cup_state new_state);
        Event make_initialize_event();
        Event make_load_event();

        Event make_client_connect_event(const Generic_client& client);
        Event make_client_disconnect_event(const Generic_client& client);
    }
}

#endif