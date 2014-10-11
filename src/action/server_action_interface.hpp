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

#ifndef SERVER_ACTION_INTERFACE_HPP
#define SERVER_ACTION_INTERFACE_HPP

#include "messages/message_listener.hpp"

#include "cup/client_player_mapping.hpp"

namespace ts
{
    namespace network
    {
        struct Client_message;
        class Server_message_interface;
    }

    namespace action
    {
        class Stage;

        class Server_action_interface
            : private messages::Message_listener<network::Client_message>
        {
        public:
            Server_action_interface(Stage* stage, const cup::Client_player_mapping* client_player_mapping, listener_set_type* listener_set);
            ~Server_action_interface();

            Server_action_interface(const Server_action_interface&) = delete;
            Server_action_interface& operator=(const Server_action_interface&) = delete;

            virtual void handle_message(const network::Client_message& message) override;

        private:
            Stage* stage_;
            const cup::Client_player_mapping* client_player_mapping_;
        };
    }
}

#endif