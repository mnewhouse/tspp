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

#pragma

#ifndef NETWORK_CLIENT_CONNECTION_HPP
#define NETWORK_CLIENT_CONNECTION_HPP

#include <SFML/Network.hpp>

#include "messages/message.hpp"

namespace ts
{
    namespace network
    {
        using messages::Message;

        enum class Connection_status
        {
            None,
            Connecting,
            Connected,
            Failed,
            Disconnected
        };

        namespace impl
        {
            class Client_connection;
        }

        class Client_connection
        {
        public:
            Client_connection();
            ~Client_connection();

            void async_connect(utf8_string remote_address, std::uint16_t remote_port);
            void disconnect();

            Connection_status connection_status() const;

            bool get_tcp_message(Message& message);
            bool get_udp_message(Message& message);

            void send_tcp_message(const Message& message);
            void send_udp_message(const Message& message);

        private:
            std::unique_ptr<impl::Client_connection> impl_;
        };
    }
}

#endif