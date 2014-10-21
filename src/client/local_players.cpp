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
#include "local_players.hpp"

#include "cup/cup_metadata.hpp"

#include "resources/player_store.hpp"
#include "resources/settings/player_settings.hpp"

std::vector<ts::cup::Player_definition> ts::client::create_local_player_definitions(const resources::Player_settings& player_settings,
                                                                                    const resources::Player_store& player_store)
{
    using cup::Player_definition;

    std::vector<Player_definition> result;

    const auto& selected_players = player_settings.selected_players();
    for (std::int32_t slot = 0; slot != selected_players.size(); ++slot)
    {
        if (auto player_handle = player_store.get_player_by_id(selected_players[slot]))
        {
            Player_definition player_def;
            player_def.color = player_handle->color;
            player_def.nickname = player_handle->name;
            player_def.id = player_handle->id;
            player_def.control_slot = slot;
            player_def.handle = 0; // Not used            
            
            result.push_back(player_def);
        }
    }

    return result;
}