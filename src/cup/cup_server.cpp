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

#ifdef TROLOLO

#include "stdinc.hpp"
#include "cup_server.hpp"

#include "network/server.hpp"

struct ts::cup::Cup_server::Impl
{
    network::Server server;
    std::unordered_map<Client_id, network::Client_handle> client_map;

    network::Client_handle get_client_by_id(Client_id client_id) const;
};

ts::network::Client_handle ts::cup::Cup_server::Impl::get_client_by_id(Client_id client_id) const
{
    auto it = client_map.find(client_id);
    if (it != client_map.end())
    {
        return it->second;
    }

    return network::Client_handle();
}

ts::cup::Cup_server::Cup_server(std::uint16_t listen_port)
: impl_(std::make_unique<Impl>())
{
    impl_->server.listen(listen_port);
}

ts::cup::Cup_server::~Cup_server()
{
}

bool ts::cup::Cup_server::get_message(Client_message& client_message) const
{
    network::Client_handle client_handle;
    if (impl_->server.get_message(client_message.message, client_handle))
    {
        client_message.client_id = client_handle->client_key();
        return true;
    }

    return false;    
}

void ts::cup::Cup_server::send_message(const Client_message& client_message) const
{

}

#endif