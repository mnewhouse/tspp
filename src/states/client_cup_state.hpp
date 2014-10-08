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

#ifndef CLIENT_CUP_STATE_HPP
#define CLIENT_CUP_STATE_HPP

#include "cup_state_base.hpp"

#include "network/client.hpp"

#include "cup/cup.hpp"
#include "cup/client_cup_interface.hpp"

namespace ts
{
    namespace states
    {
        namespace impl
        {
            struct Client_cup_state_members
            {
                Client_cup_state_members(resources::Resource_store* resource_store);

                cup::Cup cup_;
                network::Client client_;
                cup::Client_cup_interface client_cup_interface_;
            };
        }

        class Client_cup_state
            : private impl::Client_cup_state_members, public Cup_state_base
        {
        public:
            Client_cup_state(state_machine_type* state_machine, gui::Context* context,
                             resources::Resource_store* resource_store);

            virtual ~Client_cup_state();

            void async_connect(utf8_string remote_address, std::uint16_t remote_port);
            network::Connection_status connection_status() const;

            cup::Registration_status registration_status() const;
            const utf8_string& registration_error() const;

            void send_registration_request();

            virtual void update(std::size_t frame_duration) override;

        private:
        };
    }
}

#endif