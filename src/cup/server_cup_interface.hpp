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

#ifndef SERVER_CUP_INTERFACE_HPP
#define SERVER_CUP_INTERFACE_HPP

#include "network/server.hpp"

#include "cup_interface.hpp"

namespace ts
{
    namespace resources
    {
        struct Resource_store;
    }

    namespace cup
    {
        struct Stage_data;

        class Server_cup_interface
            : public Cup_interface
        {
        public:
            Server_cup_interface(Cup* cup, network::Server* server, const resources::Resource_store* resource_store);

            void update(std::size_t frame_duration);

            virtual void select_cars(const std::vector<Car_selection>& car_selection) override;
            virtual void signal_ready() override;
            virtual void write_chat_message(const utf8_string& message) override;

            void send_initialization_message(const Stage_data& stage_data);
            
        private:
            void handle_registration_request(const network::Client_message& message);
            void handle_bad_registration_request(network::Client_handle client);

            void handle_message(const network::Client_message& message);
            void handle_ready_signal(const network::Client_message& message);
            void handle_car_selection(const network::Client_message& message);
            void handle_chat_message(const network::Client_message& message);
            
            void disconnect_client(network::Client_handle client_handle);

            utf8_string client_name(network::Client_handle client) const;
            utf8_string my_name() const;

            void wait_for_everyone();
            void advance_if_ready();

            virtual void on_state_change(Cup_state old_state, Cup_state new_state) override;

            network::Server* server_;

            const resources::Resource_store* resource_store_;
            network::Client_message message_buffer_;

            std::unordered_map<std::uint32_t, std::vector<Player_handle>> client_player_mapping_;

            std::unordered_set<std::uint32_t> awaiting_clients_;
            bool awaiting_self_;
        };
    }
}

#endif
