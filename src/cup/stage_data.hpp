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

#ifndef CUP_STAGE_DATA_HPP
#define CUP_STAGE_DATA_HPP

#include "resources/car_handle.hpp"
#include "resources/track_handle.hpp"
#include "resources/player_color.hpp"
#include "resources/script_resource.hpp"

#include "controls/control.hpp"

namespace ts
{

    namespace cup
    {
        class Cup;

        struct Car_data
        {
            controls::Slot control_slot;
            resources::Car_handle car;
            resources::Player_color color;
            std::size_t start_pos;
        };

        struct Stage_data
        {
            Pointer_handle<Cup> cup;
            resources::Track_handle track_handle;

            std::vector<Car_data> cars;
            std::vector<resources::Script_handle> loaded_scripts;
        };
    }
}

#endif