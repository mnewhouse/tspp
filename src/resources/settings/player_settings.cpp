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
#include "player_settings.hpp"

ts::resources::Player_settings::Player_settings()
{
    std::fill(selected_players_.begin(), selected_players_.end(), 0);
}

const std::array<ts::resources::Player_store::unique_id, 4> ts::resources::Player_settings::selected_players() const
{
    return selected_players_;
}

void ts::resources::Player_settings::select_player(Player_store::unique_id player_id, int slot)
{
    if (slot >= 0 && slot < 4)
    {
        for (auto& id : selected_players_)
        {
            if (id == player_id) id = 0;
        }

        selected_players_[slot] = player_id;
    }
}

void ts::resources::Player_settings::deselect_player(int slot)
{
    if (slot >= 0 && slot < 4)
    {
        selected_players_[slot] = 0;
    }
}