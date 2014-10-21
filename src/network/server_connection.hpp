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

#ifndef NETWORK_SERVER_CONNECTION_HPP
#define NETWORK_SERVER_CONNECTION_HPP

#include "messages/message.hpp"

#include "network/client_handle.hpp"


namespace ts
{
    namespace network
    {
        namespace impl
        {
            class Server_connection;
        }

        class Server_connection
        {
        public:
            Server_connection();
            ~Server_connection();

            bool listen(std::uint16_t port_number);
            Client_handle accept_incoming_connection();

            bool get_tcp_message(Message& message, Client_handle& client);
            bool get_udp_message(Message& message, Remote_address& sender);

            void send_tcp_message(const Message& message, const Client_handle& client);
            void send_tcp_message_to_all(const Message& message);

            void send_udp_message(const Message& message, const Remote_address& recipient);
            void send_udp_message(const Message& message, const Client_handle& client);
            void send_udp_message_to_all(const Message& message);

            void close_client_connection(Client_handle client_handle);

        private:
            std::unique_ptr<impl::Server_connection> impl_;
        };
    }
}

#endif