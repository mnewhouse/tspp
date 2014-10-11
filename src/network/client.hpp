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

#include "message_protocol.hpp"
#include "messages/message.hpp"

namespace ts
{
    namespace network
    {
        enum class Connection_status
        {
            None,
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

            void async_connect(utf8_string remote_address, std::uint16_t remote_port);
            void disconnect();

            Connection_status connection_status() const;

            void send_message(messages::Message message, Message_protocol protocol = Message_protocol::Tcp);
            bool get_message(messages::Message& message);

            std::uint64_t generate_key();

        private:
            void async_get_messages();
            void async_connect_impl(utf8_string remote_address, std::uint16_t remote_port);
            void connection_loop();

            sf::TcpSocket tcp_socket_;
            sf::UdpSocket udp_socket_;

            struct Outgoing_message
            {
                messages::Message message;
                Message_protocol protocol;
            };

            boost::lockfree::spsc_queue<Outgoing_message, boost::lockfree::allocator<std::allocator<Outgoing_message>>> outgoing_messages_;
            boost::lockfree::spsc_queue<messages::Message, boost::lockfree::allocator<std::allocator<messages::Message>>> incoming_messages_;

            std::future<void> connect_future_;
            std::atomic<Connection_status> connection_status_ = Connection_status::None;
            std::thread message_thread_;
            
            std::mt19937_64 key_generator_;
        };
    }
}

#endif