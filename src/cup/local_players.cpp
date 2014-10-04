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

#include "cup_interface.hpp"

#include "resources/player_store.hpp"
#include "resources/settings/player_settings.hpp"

void ts::cup::add_selected_local_players(Cup_interface* cup_interface, const resources::Player_settings& player_settings,
                                         const resources::Player_store& player_store)
{
    const auto& selected_players = player_settings.selected_players();

    for (controls::Slot slot = 0; slot != selected_players.size(); ++slot)
    {
        auto player_id = selected_players[slot];

        if (auto player_handle = player_store.get_player_by_id(player_id))
        {
            cup::Player player;
            player.color = player_handle->color;
            player.id = player_handle->id;
            player.nickname = player_handle->name;
            cup_interface->add_player(player, slot);
        }        
    }
}