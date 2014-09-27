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

#ifndef NETWORK_CLIENT_HPP
#define NETWORK_CLIENT_HPP

#include <SFML/Network.hpp>

#include <boost/lockfree/spsc_queue.hpp>

#include "message.hpp"

namespace ts
{
    namespace network
    {
        enum class Connection_status
        {
            Connecting,
            Connected,
            Failed,
            Disconnected
        };

        class Client
        {
        public:
            Client();
            ~Client();

            void async_connect(sf::IpAddress remote_address, std::uint16_t remote_port, std::uint32_t timeout = 0);
            void async_get_messages();
                

            Connection_status connection_status() const;

            void send_message(Message message, Message_protocol protocol = Message_protocol::Udp);
            bool get_message(Message& message);

        private:
            void async_connect_impl(sf::IpAddress address, std::uint16_t remote_port, std::uint32_t timeout);
            void connection_loop();

            sf::TcpSocket tcp_socket_;
            sf::UdpSocket udp_socket_;

            struct Outgoing_message
            {
                Message message;
                Message_protocol protocol;
            };

            boost::lockfree::spsc_queue<Outgoing_message, boost::lockfree::allocator<std::allocator<Outgoing_message>>> outgoing_messages_;
            boost::lockfree::spsc_queue<Message, boost::lockfree::allocator<std::allocator<Message>>> incoming_messages_;

            std::future<void> connect_future_;
            std::atomic<Connection_status> connection_status_;
            std::thread message_thread_;
        };
    }
}

#endif