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
            class Server;
        }

        using messages::Message;
        class Connected_client;

        struct Client_handle
        {
        public:
            Client_handle() = default;
            explicit operator bool() const;

            std::uint32_t client_id() const;
            std::uint16_t remote_port() const;
            sf::IpAddress remote_address() const;

        private:
            friend class impl::Server;
            explicit Client_handle(const Connected_client* connected_client);

            const Connected_client* client_impl_;
        };

        inline bool operator==(Client_handle lhs, Client_handle rhs)
        {
            return lhs.client_id() == rhs.client_id();
        }
          
        inline bool operator!=(Client_handle lhs, Client_handle rhs)
        {
            return !(lhs == rhs);
        }

        inline bool operator<(Client_handle lhs, Client_handle rhs)
        {
            return lhs.client_id() < rhs.client_id();
        }

        inline bool operator>(Client_handle lhs, Client_handle rhs)
        {
            return lhs.client_id() > rhs.client_id();
        }

        inline bool operator<=(Client_handle lhs, Client_handle rhs)
        {
            return lhs.client_id() <= rhs.client_id();
        }

        inline bool operator>=(Client_handle lhs, Client_handle rhs)
        {
            return lhs.client_id() >= rhs.client_id();
        }

        struct Client_message
        {
            Client_handle client;
            Message message;
        };
    }
}

namespace std
{
    template <>
    struct hash<ts::network::Client_handle>
    {
        std::size_t operator()(const ts::network::Client_handle& client) const
        {
            return client.client_id();
        }
    };
}



#endif