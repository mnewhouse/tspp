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

#include <SFML/Network.hpp>

#include <boost/lockfree/spsc_queue.hpp>



namespace ts
{
    namespace network
    {
        class Connected_client
        {
        public:
            Connected_client(std::unique_ptr<sf::TcpSocket> socket, std::uint32_t client_key);

            sf::IpAddress remote_address() const;
            std::uint16_t remote_port() const;

            std::uint32_t client_key() const;

        private:
            std::uint32_t client_key_;
            std::unique_ptr<sf::TcpSocket> socket_;

            friend class Server;
        };

        using Client_handle = Pointer_handle<const Connected_client>;

        struct Client_message
        {
            Client_handle client;
            Message message;
        };

        class Server
        {
        public:
            Server();
            ~Server();

            Server(const Server&) = delete;
            Server& operator=(const Server&) = delete;

            bool listen(std::uint16_t port_number);
            void stop_listening();

            bool get_message(Client_message& message);
            void send_message(Client_message message, Message_protocol protocol = Message_protocol::Tcp);

            void close_client_connection(Client_handle client_handle);

        private:
            void handle_networking();
            Client_handle create_client_connection(std::unique_ptr<sf::TcpSocket> socket);
            std::uint32_t generate_client_key() const;

            void read_incoming_messages();
            void remove_closed_connections();
            void send_outgoing_messages();

            sf::TcpListener socket_listener_;
            sf::UdpSocket udp_socket_;

            std::map<std::uint32_t, Connected_client> clients_;

            using Incoming_message = Client_message;

            struct Outgoing_message
                : Client_message
            {
                Message_protocol protocol;
            };            

            boost::lockfree::spsc_queue<Incoming_message, boost::lockfree::allocator<std::allocator<Incoming_message>>> incoming_queue_;
            boost::lockfree::spsc_queue<Outgoing_message, boost::lockfree::allocator<std::allocator<Outgoing_message>>> outgoing_queue_;

            boost::lockfree::spsc_queue<Client_handle, boost::lockfree::allocator<std::allocator<Client_handle>>> closed_connections_;

            std::thread networking_thread_;
            std::atomic<bool> is_listening_;

            mutable std::mt19937 random_engine_;
        };
    }
}

#endif