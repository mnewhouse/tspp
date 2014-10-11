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

#ifndef NETWORK_SERVER_HPP
#define NETWORK_SERVER_HPP

#include "message_protocol.hpp"
#include "messages/message.hpp"

#include "network/client_handle.hpp"

namespace ts
{
    namespace network
    {
        class Server
        {
        public:
            Server();
            ~Server();

            bool listen(std::uint16_t port_number);
            void stop_listening();

            bool get_message(Client_message& client_message) const;
            void send_message(const Client_message& client_message, Message_protocol protocol = Message_protocol::Tcp) const;
            void send_message_to_all(const Message& message, Message_protocol = Message_protocol::Tcp) const;

            void close_client_connection(Client_handle client_handle);

        private:
            std::unique_ptr<impl::Server> impl_;
        };
    }
}

#endif