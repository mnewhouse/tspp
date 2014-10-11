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
#include "client_player_mapping.hpp"

std::pair<const ts::cup::Player_handle*, const ts::cup::Player_handle*> ts::cup::Client_player_mapping::get_players_by_client(std::uint32_t client_key) const
{
    auto it = mapping_.find(client_key);
    if (it == mapping_.end())
    {
        return std::make_pair(nullptr, nullptr);
    }

    const auto& players = it->second;
    return std::make_pair(players.data(), players.data() + players.size());
}


bool ts::cup::Client_player_mapping::is_player_mapped_to_client(Player_handle player, std::uint32_t client_key) const
{
    auto it = mapping_.find(client_key);
    if (it != mapping_.end())
    {
        const auto& players = it->second;
        return std::find(players.begin(), players.end(), player) != players.end();
    }

    return false;
}

void ts::cup::Client_player_mapping::add_player(Player_handle player, std::uint32_t client_key)
{
    mapping_[client_key].push_back(player);
}

void ts::cup::Client_player_mapping::remove_players_by_client(std::uint32_t client_key)
{
    mapping_.erase(client_key);
}