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
#include "client_map.hpp"

#include "cup/cup.hpp"
#include "network/server_connection.hpp"

ts::server::Client_map::Client_map(cup::Cup* cup, network::Server_connection* server_connection)
: cup_(cup),
  server_connection_(server_connection)
{
}

void ts::server::Client_map::register_client(const Generic_client& client, const std::vector<cup::Player_definition>& players)
{
    auto& player_list = player_mapping_[client];

    for (auto player : players)
    {
        if (client.type() != Generic_client::Local)
        {
            // The cup only needs to know the control slot if the player is local
            player.control_slot = controls::invalid_slot;
        }

        auto player_handle = cup_->add_player(player);
        player_list.push_back(player_handle);
    }

    client_list_.push_back(client);
}

void ts::server::Client_map::disconnect_client(const Generic_client& client)
{
    player_mapping_.erase(client);
    client_list_.erase(std::remove(client_list_.begin(), client_list_.end(), client), client_list_.end());

    if (client.type() == Generic_client::Handle)
    {
        server_connection_->close_client_connection(client.remote_handle());
    }
}

ts::Range<const ts::cup::Player_handle*> ts::server::Client_map::get_players_by_client(const Generic_client& client) const
{
    using range_type = Range<const cup::Player_handle*>;

    auto it = player_mapping_.find(client);
    if (it == player_mapping_.end())
    {
        return range_type(nullptr, nullptr);
    }

    auto& list = it->second;
    return range_type(list.data(), list.data() + list.size());
}

const std::vector<ts::server::Generic_client>& ts::server::Client_map::client_list() const
{
    return client_list_;
}

bool ts::server::Client_map::client_exists(const Generic_client& client) const
{
    return player_mapping_.find(client) != player_mapping_.end();
}