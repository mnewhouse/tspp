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

#ifndef CONNECTED_CLIENT_HPP
#define CONNECTED_CLIENT_HPP

#include "messages/message.hpp"

#include <SFML/Network/IpAddress.hpp>

namespace ts
{
    namespace network
    {
        using messages::Message;

        namespace impl
        {
            class Server_connection;
        }

        class Connected_client;

        struct Remote_address
        {
            sf::IpAddress address;
            std::uint16_t port;
        };

        struct Client_handle
        {
        public:
            Client_handle() = default;
            explicit operator bool() const;

            std::uint32_t client_id() const;
            std::uint16_t remote_port() const;
            sf::IpAddress remote_address() const;

            bool operator==(Client_handle rhs) const;
            bool operator!=(Client_handle rhs) const;
            bool operator<(Client_handle rhs) const;
            bool operator>(Client_handle rhs) const;
            bool operator<=(Client_handle rhs) const;
            bool operator>=(Client_handle rhs) const;

        private:
            friend class impl::Server_connection;
            explicit Client_handle(const Connected_client* connected_client);

            const Connected_client* client_impl_ = nullptr;
        };

        inline bool Client_handle::operator==(Client_handle rhs) const
        {
            return client_impl_ == rhs.client_impl_;
        }
        
        inline bool Client_handle::operator!=(Client_handle rhs) const
        {
            return client_impl_ != rhs.client_impl_;
        }

        inline bool Client_handle::operator<(Client_handle rhs) const
        {
            return client_impl_ < rhs.client_impl_;
        }

        inline bool Client_handle::operator>(Client_handle rhs) const
        {
            return client_impl_ > rhs.client_impl_;
        }

        inline bool Client_handle::operator<=(Client_handle rhs) const
        {
            return client_impl_ <= rhs.client_impl_;
        }

        inline bool Client_handle::operator>=(Client_handle rhs) const
        {
            return client_impl_ >= rhs.client_impl_;
        }

        inline bool operator==(const Remote_address& lhs, const Remote_address& rhs)
        {
            return lhs.address == rhs.address && lhs.port == rhs.port;
        }
               
        inline bool operator!=(const Remote_address& lhs, const Remote_address& rhs)
        {
            return !(lhs == rhs);
        }

        inline bool operator<(const Remote_address& lhs, const Remote_address& rhs)
        {
            if (lhs.address == rhs.address)
            {
                return lhs.port == rhs.port;
            }

            return lhs.address < rhs.address;
        }

        inline bool operator>(const Remote_address& lhs, const Remote_address& rhs)
        {
            return rhs < lhs;
        }

        inline bool operator<=(const Remote_address& lhs, const Remote_address& rhs)
        {
            return !(lhs > rhs);
        }

        inline bool operator>=(const Remote_address& lhs, const Remote_address& rhs)
        {
            return !(lhs < rhs);
        }
    }
}

namespace std
{
    template <>
    struct hash<ts::network::Client_handle>
    {
        std::size_t operator()(const ts::network::Client_handle& client) const
        {
            return client ? client.client_id() : 0;
        }
    };

    template <>
    struct hash<ts::network::Remote_address>
    {
        std::size_t operator()(const ts::network::Remote_address& remote_address) const
        {
            return remote_address.address.toInteger() ^ remote_address.port;
        }
    };
}



#endif