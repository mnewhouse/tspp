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

#include "stdinc.hpp"
#include "server.hpp"

ts::network::Server::Server()
: incoming_queue_(64, std::allocator<Incoming_message>()),
  outgoing_queue_(64, std::allocator<Outgoing_message>()),
  closed_connections_(64, std::allocator<Client_handle>())
{
    random_engine_.seed(std::random_device{}());
}

ts::network::Server::~Server()
{
    stop_listening();

    if (networking_thread_.joinable())
    {
        networking_thread_.join();
    }
}

bool ts::network::Server::get_message(Client_message& client_message)
{
    return incoming_queue_.pop(client_message);
}

void ts::network::Server::send_message(Client_message client_message, Message_protocol protocol)
{
    Outgoing_message message;
    message.client = client_message.client;
    message.message = std::move(client_message.message);
    message.protocol = protocol;

    outgoing_queue_.push(message);
}

bool ts::network::Server::listen(std::uint16_t port_number)
{
    auto status = socket_listener_.listen(port_number);
    auto udp_status = udp_socket_.bind(port_number);

    if (status == sf::Socket::Done && udp_status == sf::Socket::Done)
    {
        socket_listener_.setBlocking(false);
        udp_socket_.setBlocking(false);

        is_listening_ = true;
        networking_thread_ = std::thread([this]() { handle_networking(); });
        return true;
    }
    
    return false;
}

void ts::network::Server::stop_listening()
{
    is_listening_ = false;
}

std::uint32_t ts::network::Server::generate_client_key() const
{
    std::uniform_int_distribution<std::uint32_t> key_dist(0x10000, std::numeric_limits<std::uint32_t>::max());

    auto client_key = key_dist(random_engine_);
    while (clients_.find(client_key) != clients_.end())
    {
        client_key = key_dist(random_engine_);
    }

    return client_key;
}

ts::network::Client_handle ts::network::Server::create_client_connection(std::unique_ptr<sf::TcpSocket> socket)
{
    auto client_key = generate_client_key();    

    auto insert_result = clients_.emplace(std::piecewise_construct, std::forward_as_tuple(client_key), 
                                                                    std::forward_as_tuple(std::move(socket), client_key));

    return Client_handle(&insert_result.first->second);
}

void ts::network::Server::close_client_connection(Client_handle client_handle)
{
    closed_connections_.push(client_handle);
}

void ts::network::Server::read_incoming_messages()
{
    Client_message client_message;

    sf::Packet packet;

    for (auto& client_info : clients_)
    {
        const auto& client = client_info.second;
        while (client.socket_->receive(packet) == sf::Socket::Done)
        {
            client_message.client.set(&client);
            client_message.message.assign(static_cast<const std::uint8_t*>(packet.getData()), packet.getDataSize());

            incoming_queue_.push(client_message);
        }
    }
}

void ts::network::Server::send_outgoing_messages()
{
    sf::Packet packet;

    Outgoing_message outgoing_message;
    while (outgoing_queue_.pop(outgoing_message))
    {
        const auto& message = outgoing_message.message;
        auto client = outgoing_message.client;
        auto protocol = outgoing_message.protocol;

        packet.clear();
        packet.append(message.data(), message.size());

        if (protocol == Message_protocol::Tcp)
        {
            client->socket_->send(packet);
        }

        else if (protocol == Message_protocol::Udp)
        {
            udp_socket_.send(packet, client->remote_address(), client->remote_port());
        }       
    }
}

void ts::network::Server::remove_closed_connections()
{
    Client_handle client;
    while (closed_connections_.pop(client))
    {
        auto it = clients_.find(client->client_key_);
        if (it != clients_.end())
        {
            clients_.erase(it);
        }
    }
}

void ts::network::Server::handle_networking()
{
    sf::Packet packet;

    auto make_nonblocking_socket = []()
    {
        auto result = std::make_unique<sf::TcpSocket>();
        result->setBlocking(false);
        return result;
    };

    auto pending_socket = make_nonblocking_socket();

    while (is_listening_)
    {
        while (socket_listener_.accept(*pending_socket) == sf::Socket::Done)
        {
            create_client_connection(std::move(pending_socket));

            pending_socket = make_nonblocking_socket();
        }

        read_incoming_messages();
        send_outgoing_messages();
        remove_closed_connections();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

ts::network::Connected_client::Connected_client(std::unique_ptr<sf::TcpSocket> socket, std::uint32_t client_key)
: socket_(std::move(socket)),
  client_key_(client_key)
{
}

std::uint32_t ts::network::Connected_client::client_key() const
{
    return client_key_;
}

sf::IpAddress ts::network::Connected_client::remote_address() const
{
    return socket_->getRemoteAddress();
}

std::uint16_t ts::network::Connected_client::remote_port() const
{
    return socket_->getRemotePort();
}