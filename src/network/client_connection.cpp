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
#include "client_connection.hpp"

namespace ts
{
    namespace network
    {
        namespace impl
        {
            class Client_connection
            {
            public:
                Client_connection();

                void async_connect(utf8_string remote_address, std::uint16_t remote_port);

                void disconnect();
                Connection_status connection_status() const;

                bool get_tcp_message(Message& message);
                bool get_udp_message(Message& message);

                void send_tcp_message(const Message& message);
                void send_udp_message(const Message& message);

            private:
                void async_connect_impl(utf8_string remote_address, std::uint16_t remote_port);

                std::uint64_t generate_key();

                sf::TcpSocket tcp_socket_;
                sf::UdpSocket udp_socket_;
                sf::Packet packet_buffer_;

                std::future<void> connect_future_;
                std::atomic<Connection_status> connection_status_ = Connection_status::None;
            
                std::mt19937_64 key_generator_;
            };
        }
    }
}

ts::network::impl::Client_connection::Client_connection()
{
    std::random_device random_device;
    std::uint64_t seed = 0;
    seed |= random_device();
    seed |= static_cast<std::uint64_t>(random_device()) << 32;

    key_generator_.seed(seed);
}

std::uint64_t ts::network::impl::Client_connection::generate_key()
{
    return key_generator_();
}

ts::network::Connection_status ts::network::impl::Client_connection::connection_status() const
{
    return connection_status_;
}

void ts::network::impl::Client_connection::async_connect(utf8_string remote_address, std::uint16_t remote_port)
{
    connection_status_ = Connection_status::Connecting;

    auto callable = [=]()
    {
        async_connect_impl(remote_address, remote_port);
    };

    connect_future_ = std::async(std::launch::async, callable);
}

void ts::network::impl::Client_connection::disconnect()
{
    connection_status_ = Connection_status::Disconnected;

    tcp_socket_.disconnect();
    udp_socket_.unbind();
}

void ts::network::impl::Client_connection::async_connect_impl(utf8_string remote_address, std::uint16_t remote_port)
{
    auto result = tcp_socket_.connect(remote_address.string(), remote_port);
    if (result == sf::Socket::Done)
    {
        udp_socket_.bind(tcp_socket_.getLocalPort());

        tcp_socket_.setBlocking(false);        
        udp_socket_.setBlocking(false);

        connection_status_ = Connection_status::Connected;
    }

    else if (result == sf::Socket::Error || result == sf::Socket::Disconnected)
    {
        connection_status_ = Connection_status::Failed;
    }
}

void ts::network::impl::Client_connection::send_tcp_message(const Message& message)
{
    packet_buffer_.clear();
    packet_buffer_.append(message.data(), message.size());

    tcp_socket_.send(packet_buffer_);
}

void ts::network::impl::Client_connection::send_udp_message(const Message& message)
{
    packet_buffer_.clear();
    packet_buffer_.append(message.data(), message.size());

    udp_socket_.send(packet_buffer_, tcp_socket_.getRemoteAddress(), tcp_socket_.getRemotePort());
}

bool ts::network::impl::Client_connection::get_tcp_message(Message& message)
{
    if (tcp_socket_.receive(packet_buffer_) == sf::Socket::Done)
    {
        message.assign(reinterpret_cast<const std::uint8_t*>(packet_buffer_.getData()), packet_buffer_.getDataSize());
        return true;
    }

    return false;
}

bool ts::network::impl::Client_connection::get_udp_message(Message& message)
{
    sf::IpAddress remote_address;
    std::uint16_t remote_port;
    if (udp_socket_.receive(packet_buffer_, remote_address, remote_port) == sf::Socket::Done)
    {
        message.assign(reinterpret_cast<const std::uint8_t*>(packet_buffer_.getData()), packet_buffer_.getDataSize());
        return true;
    }

    return false;
}

ts::network::Client_connection::Client_connection()
: impl_(std::make_unique<impl::Client_connection>())
{
}

ts::network::Client_connection::~Client_connection()
{
}

ts::network::Connection_status ts::network::Client_connection::connection_status() const
{
    return impl_->connection_status();
}


void ts::network::Client_connection::send_tcp_message(const Message& message)
{
    impl_->send_tcp_message(message);
}

void ts::network::Client_connection::send_udp_message(const Message& message)
{
    impl_->send_udp_message(message);
}

bool ts::network::Client_connection::get_tcp_message(Message& message)
{
    return impl_->get_tcp_message(message);
}

bool ts::network::Client_connection::get_udp_message(Message& message)
{
    return impl_->get_udp_message(message);
}

void ts::network::Client_connection::async_connect(utf8_string remote_address, std::uint16_t remote_port)
{
    impl_->async_connect(std::move(remote_address), remote_port);
}