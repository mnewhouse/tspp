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

#ifndef SERVER_INTERACTION_EVENTS_HPP
#define SERVER_INTERACTION_EVENTS_HPP

#include "server_interaction_listener.hpp"

namespace ts
{
    namespace server
    {
        class Interaction_interface;
        class Cup_script_interface;

        class Interaction_event_listener
            : public Interaction_listener
        {
        public:
            Interaction_event_listener(Interaction_interface* interaction_interface, Cup_script_interface* script_interface);
            
            virtual void on_client_connect(const Generic_client& client) override;
            virtual void on_client_disconnect(const Generic_client& client) override;

        private:
            Interaction_interface* interaction_interface_;
            Cup_script_interface* cup_script_interface_;
        };
    }
}

#endif