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
#include "server_connection.hpp"

#include <SFML/Network.hpp>

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

            friend class impl::Server_connection;
        };
    }    
}


class ts::network::impl::Server_connection
{
public:
    Server_connection();
    ~Server_connection();

    bool listen(std::uint16_t port_number);
    void stop_listening();

    Client_handle accept_incoming_connection();

    bool get_tcp_message(Message& message, Client_handle& client);
    bool get_udp_message(Message& message, Remote_address& sender);

    void send_tcp_message(const Message& message, const Client_handle& client);
    void send_tcp_message_to_all(const Message& message);

    void send_udp_message(const Message& message, const Client_handle& client);
    void send_udp_message(const Message& message, const Remote_address& recipient);
    void send_udp_message_to_all(const Message& message);

    void close_client_connection(Client_handle client_handle);

private:
    void handle_networking();
    Client_handle create_client_connection(std::unique_ptr<sf::TcpSocket> socket);
    std::unique_ptr<sf::TcpSocket> create_tcp_connection();
    std::uint32_t generate_client_key() const;

    bool is_listening_ = false;
    sf::TcpListener socket_listener_;    
    sf::UdpSocket udp_socket_;
    std::unique_ptr<sf::TcpSocket> incoming_connection_;
    sf::Packet packet_buffer_;

    std::map<std::uint32_t, Connected_client> clients_;
    mutable std::mt19937 random_engine_;

};

ts::network::impl::Server_connection::Server_connection()
: incoming_connection_(create_tcp_connection())
{
    random_engine_.seed(std::random_device{}());
}

ts::network::impl::Server_connection::~Server_connection()
{
    stop_listening();
}

std::unique_ptr<sf::TcpSocket> ts::network::impl::Server_connection::create_tcp_connection()
{
    auto result = std::make_unique<sf::TcpSocket>();
    result->setBlocking(false);
    return result;
}

ts::network::Client_handle ts::network::impl::Server_connection::accept_incoming_connection()
{
    if (is_listening_ && socket_listener_.accept(*incoming_connection_) == sf::Socket::Done)
    {
        auto new_socket = create_tcp_connection();
        std::swap(new_socket, incoming_connection_);
        return create_client_connection(std::move(new_socket));
    }

    return Client_handle();
}

bool ts::network::impl::Server_connection::get_tcp_message(Message& message, Client_handle& client_handle)
{
    for (auto& client : clients_)
    {
        if (client.second.socket_->receive(packet_buffer_) == sf::Socket::Done)
        {
            message.assign(reinterpret_cast<const std::uint8_t*>(packet_buffer_.getData()), packet_buffer_.getDataSize());
            client_handle = Client_handle(&client.second);
            return true;
        }
    }

    return false;
}

void ts::network::impl::Server_connection::send_tcp_message(const Message& message, const Client_handle& client)
{
    packet_buffer_.clear();
    packet_buffer_.append(message.data(), message.size());

    std::cout << "Outgoing message " << message.type() << std::endl;

    client.client_impl_->socket_->send(packet_buffer_);
}

void ts::network::impl::Server_connection::send_tcp_message_to_all(const Message& message)
{
    packet_buffer_.clear();
    packet_buffer_.append(message.data(), message.size());

    for (auto& client : clients_)
    {
        client.second.socket_->send(packet_buffer_);
    }
}

void ts::network::impl::Server_connection::send_udp_message(const Message& message, const Client_handle& client)
{
    send_udp_message(message, { client.remote_address(), client.remote_port() });
}

void ts::network::impl::Server_connection::send_udp_message(const Message& message, const Remote_address& recipient)
{
    packet_buffer_.clear();
    packet_buffer_.append(message.data(), message.size());

    udp_socket_.send(packet_buffer_, recipient.address, recipient.port);
}

void ts::network::impl::Server_connection::send_udp_message_to_all(const Message& message)
{
    packet_buffer_.clear();
    packet_buffer_.append(message.data(), message.size());

    for (auto& client : clients_)
    {
        udp_socket_.send(packet_buffer_, client.second.remote_address(), client.second.remote_port());
    }
}

bool ts::network::impl::Server_connection::get_udp_message(Message& message, Remote_address& sender)
{
    if (udp_socket_.receive(packet_buffer_, sender.address, sender.port) == sf::Socket::Done)
    {
        message.assign(reinterpret_cast<const std::uint8_t*>(packet_buffer_.getData()), packet_buffer_.getDataSize());
        return true;
    }

    return false;
}

bool ts::network::impl::Server_connection::listen(std::uint16_t port_number)
{
    auto status = socket_listener_.listen(port_number);
    auto udp_status = udp_socket_.bind(port_number);

    if (status == sf::Socket::Done && udp_status == sf::Socket::Done)
    {
        socket_listener_.setBlocking(false);
        udp_socket_.setBlocking(false);
        is_listening_ = true;
        return true;
    }

    socket_listener_.close();
    udp_socket_.unbind();    
    return false;
}

void ts::network::impl::Server_connection::stop_listening()
{
    socket_listener_.close();
    udp_socket_.unbind();

    is_listening_ = false;
}

std::uint32_t ts::network::impl::Server_connection::generate_client_key() const
{
    std::uniform_int_distribution<std::uint32_t> key_dist(0x10000, std::numeric_limits<std::uint32_t>::max());

    auto client_key = key_dist(random_engine_);
    while (clients_.find(client_key) != clients_.end())
    {
        client_key = key_dist(random_engine_);
    }

    return client_key;
}

ts::network::Client_handle ts::network::impl::Server_connection::create_client_connection(std::unique_ptr<sf::TcpSocket> socket)
{
    auto client_key = generate_client_key();    

    auto insert_result = clients_.emplace(std::piecewise_construct, std::forward_as_tuple(client_key), 
                                                                    std::forward_as_tuple(std::move(socket), client_key));

    return Client_handle(&insert_result.first->second);
}

void ts::network::impl::Server_connection::close_client_connection(Client_handle client_handle)
{
    
}


ts::network::Server_connection::Server_connection()
: impl_(std::make_unique<impl::Server_connection>())
{
}

ts::network::Server_connection::~Server_connection()
{
}

bool ts::network::Server_connection::listen(std::uint16_t port_number)
{
    return impl_->listen(port_number);
}

bool ts::network::Server_connection::get_tcp_message(Message& message, Client_handle& client)
{
    return impl_->get_tcp_message(message, client);
}

void ts::network::Server_connection::send_tcp_message(const Message& message, const Client_handle& client)
{
    impl_->send_tcp_message(message, client);
}

void ts::network::Server_connection::send_tcp_message_to_all(const Message& message)
{
    impl_->send_tcp_message_to_all(message);
}

bool ts::network::Server_connection::get_udp_message(Message& message, Remote_address& sender)
{
    return impl_->get_udp_message(message, sender);
}

void ts::network::Server_connection::send_udp_message(const Message& message, const Client_handle& client)
{
    impl_->send_udp_message(message, client);
}

void ts::network::Server_connection::send_udp_message(const Message& message, const Remote_address& recipient)
{
    impl_->send_udp_message(message, recipient);
}

void ts::network::Server_connection::send_udp_message_to_all(const Message& message)
{
    impl_->send_udp_message_to_all(message);
}

void ts::network::Server_connection::close_client_connection(Client_handle client)
{
    impl_->close_client_connection(client);
}

ts::network::Client_handle ts::network::Server_connection::accept_incoming_connection()
{
    return impl_->accept_incoming_connection();
}

ts::network::Client_handle::Client_handle(const Connected_client* client)
  : client_impl_(client)
{
}

std::uint32_t ts::network::Client_handle::client_id() const
{
    return client_impl_->client_key();
}

std::uint16_t ts::network::Client_handle::remote_port() const
{
    return client_impl_->remote_port();
}

sf::IpAddress ts::network::Client_handle::remote_address() const
{
    return client_impl_->remote_address();
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

ts::network::Client_handle::operator bool() const
{
    return client_impl_ != nullptr;
}