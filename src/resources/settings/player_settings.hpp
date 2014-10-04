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

#ifndef PLAYER_SETTINGS_HPP
#define PLAYER_SETTINGS_HPP

#include "resources/player_store.hpp"

namespace ts
{
    namespace resources
    {        
        class Player_settings
        {
        public:
            Player_settings();

            const std::array<Player_store::unique_id, 4> selected_players() const;

            void select_player(Player_store::unique_id, int slot);
            void deselect_player(int slot);

        private:
            std::array<Player_store::unique_id, 4> selected_players_;
        };
    }
}

#endif