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

#ifndef CUP_LOCAL_PLAYERS_HPP
#define CUP_LOCAL_PLAYERS_HPP

namespace ts
{
    namespace resources
    {
        class Player_settings;
        class Player_store;
    }

    namespace cup
    {
        class Cup_interface;

        void add_selected_local_players(Cup_interface* cup_interface, const resources::Player_settings& player_settings,
                                        const resources::Player_store& player_store);
    }
}

#endif
