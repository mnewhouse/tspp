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

#ifndef CUP_STATE_HPP
#define CUP_STATE_HPP

#include "player.hpp"

namespace ts
{
    namespace cup
    {
        enum class Cup_state
        {
            Registering,
            Cup,
            Car_selection,
            Awaiting_initialization,
            Initializing,
            Action,
            End
        };

        enum class Cup_type
        {
            Local,
            Server,
            Remote
        };

        using Player_id = std::uint16_t;

        struct Player_definition
            : public Player
        {
            Player_id handle; 
        };

        using Player_handle = Pointer_handle<const Player_definition>;

        struct Car_selection
        {
            Player_id player_id;
            std::uint32_t car_index;
        };
    }
}

#endif
