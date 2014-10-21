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
#include "generic_client.hpp"

ts::server::Generic_client::Generic_client()
: type_(None)
{
}

ts::server::Generic_client::Generic_client(Local_client_t)
: type_(Local)
{
}

ts::server::Generic_client::Generic_client(network::Client_handle client_handle)
: type_(Handle),
  remote_handle_(client_handle)
{
}

ts::server::Generic_client::Generic_client(network::Remote_address remote_address)
: type_(Address),
  remote_address_(remote_address)
{
}

ts::server::Generic_client::Type ts::server::Generic_client::type() const
{
    return type_;
}

const ts::network::Client_handle& ts::server::Generic_client::remote_handle() const
{
    return remote_handle_;
}

const ts::network::Remote_address& ts::server::Generic_client::remote_address() const
{
    return remote_address_;
}

bool ts::server::operator==(const Generic_client& lhs, const Generic_client& rhs)
{
    if (lhs.type() != rhs.type()) return false;

    switch (lhs.type())
    {
    case Generic_client::Handle:
        return lhs.remote_handle() == rhs.remote_handle();

    case Generic_client::Address:
        return lhs.remote_address() == rhs.remote_address();

    default:
        return true;
    }
}

bool ts::server::operator!=(const Generic_client& lhs, const Generic_client& rhs)
{
    return !(lhs == rhs);
}

bool ts::server::operator<(const Generic_client& lhs, const Generic_client& rhs)
{
    if (lhs.type() == rhs.type())
    {
        switch (lhs.type())
        {
        case Generic_client::Handle:
            return lhs.remote_handle() == rhs.remote_handle();

        case Generic_client::Address:
            return lhs.remote_address() < rhs.remote_address();

        default:
            return false;
        }
    }

    return lhs.type() < rhs.type();
}

bool ts::server::operator>(const Generic_client& lhs, const Generic_client& rhs)
{
    return rhs < lhs;
}

bool ts::server::operator<=(const Generic_client& lhs, const Generic_client& rhs)
{
    return !(lhs > rhs);
}

bool ts::server::operator>=(const Generic_client& lhs, const Generic_client& rhs)
{
    return !(lhs < rhs);
}


std::size_t std::hash<ts::server::Generic_client>::operator()(const ts::server::Generic_client& client) const
{
    using ts::server::Generic_client;

    switch (client.type())
    {
    case Generic_client::Handle:
        return client.remote_handle().client_id();

    case Generic_client::Address:
        return std::hash<ts::network::Remote_address>{}(client.remote_address());

    default:
        return client.type();
    }
}