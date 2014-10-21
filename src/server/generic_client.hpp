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

#ifndef GENERIC_CLIENT_HPP
#define GENERIC_CLIENT_HPP

#include "network/client_handle.hpp"

namespace ts
{
    namespace server
    {
        struct Local_client_t {};
        static const Local_client_t local_client;

        class Generic_client
        {
        public:
            explicit Generic_client(Local_client_t);
            explicit Generic_client(network::Client_handle client_handle);
            explicit Generic_client(network::Remote_address recipient);
            Generic_client();

            enum Type
            {
                None,
                Local,
                Handle,
                Address,
            };

            Type type() const;
            const network::Client_handle& remote_handle() const;
            const network::Remote_address& remote_address() const;

        private:
            Type type_;
            network::Client_handle remote_handle_;
            network::Remote_address remote_address_;
        };

        bool operator==(const Generic_client& lhs, const Generic_client& rhs);
        bool operator!=(const Generic_client& lhs, const Generic_client& rhs);
        bool operator<(const Generic_client& lhs, const Generic_client& rhs);
        bool operator>(const Generic_client& lhs, const Generic_client& rhs);
        bool operator<=(const Generic_client& lhs, const Generic_client& rhs);
        bool operator>=(const Generic_client& lhs, const Generic_client& rhs);
    }
}

namespace std
{
    template <>
    struct hash<ts::server::Generic_client>
    {
    public:
        std::size_t operator()(const ts::server::Generic_client& client) const;
    };
}

#endif