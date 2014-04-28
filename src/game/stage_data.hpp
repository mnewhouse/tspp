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

#ifndef GAME_STAGE_DATA_HPP
#define GAME_STAGE_DATA_HPP

#include "resources/car_definition.hpp"
#include "resources/track_handle.hpp"

#include "controls/control.hpp"

namespace ts
{

    namespace game
    {

        struct Stage_data
        {
            struct Player_data;

            resources::Track_handle track_handle;

            std::vector<Player_data> players;
        };

        struct Stage_data::Player_data
        {
            controls::Slot control_slot;
            resources::Car_definition car;
            std::size_t start_pos;
        };

    }

}

#endif