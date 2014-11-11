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

#ifndef CLIENT_MAP_HPP
#define CLIENT_MAP_HPP

#include "generic_client.hpp"

#include "cup/cup_metadata.hpp"

namespace ts
{
    namespace network
    {
        class Server_connection;
    }

    namespace cup
    {
        class Cup_controller;
        struct Player_definition;
    }

    namespace server
    {
        class Client_map
        {
        public:
            Client_map(cup::Cup_controller* cup_controller, network::Server_connection* server_connection);

            void disconnect_client(const Generic_client& client_handle);
            void register_client(const Generic_client& client_handle);
            cup::Player_handle register_player(const Generic_client& client_handle, cup::Player_definition player_definition);
            
            bool is_player_mapped_to_client(cup::Player_handle player, const Generic_client& client) const;
            Range<const cup::Player_handle*> get_players_by_client(const Generic_client& client) const;

            const std::vector<Generic_client>& client_list() const;
            bool client_exists(const Generic_client& client) const;

            utf8_string client_name(const Generic_client& client) const;

        private:
            std::unordered_map<Generic_client, std::vector<cup::Player_handle>> player_mapping_;
            std::vector<Generic_client> client_list_;

            cup::Cup_controller* cup_controller_;
            network::Server_connection* server_connection_;            
        };
    }
}

#endif