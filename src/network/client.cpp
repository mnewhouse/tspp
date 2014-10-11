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
#include "client.hpp"

namespace ts
{
    namespace network
    {
        using messages::Message;
    }
}

ts::network::Client::Client()
: incoming_messages_(64, std::allocator<Message>()),
  outgoing_messages_(64, std::allocator<Outgoing_message>())
{
    std::random_device random_device;
    std::uint64_t seed = 0;
    seed |= random_device();
    seed |= static_cast<std::uint64_t>(random_device()) << 32;

    key_generator_.seed(seed);
}

ts::network::Client::~Client()
{
    connection_status_ = Connection_status::Disconnected;

    if (message_thread_.joinable())
    {
        message_thread_.join();
    }

    connect_future_ = {};
}

std::uint64_t ts::network::Client::generate_key()
{
    return key_generator_();
}

void ts::network::Client::async_connect(utf8_string remote_address, std::uint16_t remote_port)
{
    connection_status_ = Connection_status::Connecting;

    auto callable = [=]()
    {
        async_connect_impl(remote_address, remote_port);
    };

    connect_future_ = std::async(std::launch::async, callable);
    async_get_messages();
}

void ts::network::Client::disconnect()
{
    connection_status_ = Connection_status::Disconnected;
}

void ts::network::Client::async_get_messages()
{
    auto callable = [=]()
    {
        connection_loop();
    };

    message_thread_ = std::thread(callable);
}

ts::network::Connection_status ts::network::Client::connection_status() const
{
    return connection_status_;
}

void ts::network::Client::async_connect_impl(utf8_string remote_address, std::uint16_t remote_port)
{
    udp_socket_.bind(sf::Socket::AnyPort);
    auto result = tcp_socket_.connect(remote_address.string(), remote_port);
    if (result == sf::Socket::Done)
    {
        tcp_socket_.setBlocking(false);
        udp_socket_.setBlocking(false);

        connection_status_ = Connection_status::Connected;
    }

    else if (result == sf::Socket::Error || result == sf::Socket::Disconnected)
    {
        connection_status_ = Connection_status::Failed;
    }
}

void ts::network::Client::connection_loop()
{
    while (connection_status_ == Connection_status::Connecting)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }

    if (connection_status_ != Connection_status::Connected)
    {
        return;
    }

    Message incoming_message;
    Outgoing_message outgoing_message;
    sf::Packet packet;

    sf::IpAddress remote_address;
    std::uint16_t remote_port;

    while (connection_status_ == Connection_status::Connected)
    {
        if (tcp_socket_.receive(packet) == sf::Socket::Done)
        {
            incoming_message.assign(static_cast<const std::uint8_t*>(packet.getData()), packet.getDataSize());
            incoming_messages_.push(incoming_message);
        }

        else if (udp_socket_.receive(packet, remote_address, remote_port) == sf::Socket::Done)
        {
            if (remote_address == tcp_socket_.getRemoteAddress() && remote_port == tcp_socket_.getRemotePort())
            {
                incoming_message.assign(static_cast<const std::uint8_t*>(packet.getData()), packet.getDataSize());
                incoming_messages_.push(incoming_message);
            }
        }

        while (outgoing_messages_.pop(outgoing_message))
        {
            packet.clear();
            packet.append(outgoing_message.message.data(), outgoing_message.message.size());

            if (outgoing_message.protocol == Message_protocol::Tcp)
            {
                tcp_socket_.send(packet);
            }

            else
            {
                udp_socket_.send(packet, tcp_socket_.getRemoteAddress(), tcp_socket_.getRemotePort());
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    udp_socket_.unbind();
    tcp_socket_.disconnect();
}

void ts::network::Client::send_message(Message message, Message_protocol protocol)
{
    Outgoing_message outgoing_message;
    outgoing_message.message = std::move(message);
    outgoing_message.protocol = protocol;

    outgoing_messages_.push(outgoing_message);
}

bool ts::network::Client::get_message(Message& message)
{
    return incoming_messages_.pop(message);
}