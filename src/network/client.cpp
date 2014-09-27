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

ts::network::Client::~Client()
{
    connection_status_ = Connection_status::Disconnected;

    if (message_thread_.joinable())
    {
        message_thread_.join();
    }
}

void ts::network::Client::async_connect(sf::IpAddress ip_address, std::uint16_t remote_port, std::uint32_t timeout)
{
    connection_status_ = Connection_status::Connecting;

    auto callable = [=]()
    {
        async_connect_impl(ip_address, remote_port, timeout);
    };

    connect_future_ = std::async(std::launch::async, callable);
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

void ts::network::Client::async_connect_impl(sf::IpAddress ip_address, std::uint16_t remote_port, std::uint32_t timeout)
{
    udp_socket_.bind(sf::Socket::AnyPort);
    auto result = tcp_socket_.connect(ip_address, remote_port, sf::milliseconds(timeout));
    if (result == sf::Socket::Done)
    {
        connection_status_ = Connection_status::Connected;
    }

    else if (result == sf::Socket::Error)
    {
        connection_status_ = Connection_status::Failed;
    }    
}

void ts::network::Client::connection_loop()
{
    sf::SocketSelector selector;
    selector.add(tcp_socket_);
    selector.add(udp_socket_);

    Message incoming_message;
    Outgoing_message outgoing_message;

    while (connection_status_ == Connection_status::Connected)
    {
        sf::Packet packet;
        if (selector.isReady(tcp_socket_))
        {
            tcp_socket_.receive(packet);
            incoming_message.assign(static_cast<const std::uint8_t*>(packet.getData()), packet.getDataSize());
            incoming_messages_.push(incoming_message);
        }

        else if (selector.isReady(udp_socket_))
        {
            sf::IpAddress remote_address;
            std::uint16_t remote_port;

            udp_socket_.receive(packet, remote_address, remote_port);
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
    }
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