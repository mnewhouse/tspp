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

#ifndef CLIENT_PLAYER_MAPPING
#define CLIENT_PLAYER_MAPPING

#include "cup_metadata.hpp"

namespace ts
{
    namespace cup
    {
        class Client_player_mapping
        {
        public:
            std::pair<const Player_handle*, const Player_handle*> get_players_by_client(std::uint32_t client_key) const;
            bool is_player_mapped_to_client(Player_handle player, std::uint32_t client_key) const;

            void add_player(Player_handle player, std::uint32_t client_key);
            void remove_players_by_client(std::uint32_t client_key);           

        private:
            std::unordered_map<std::uint32_t, std::vector<Player_handle>> mapping_;
        };
    }
}

#endif