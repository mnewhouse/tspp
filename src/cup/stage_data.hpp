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

#ifndef ACTION_STAGE_DATA_HPP
#define ACTION_STAGE_DATA_HPP

#include "cup_metadata.hpp"

#include "resources/car_model.hpp"
#include "resources/track_handle.hpp"
#include "resources/player_definition.hpp"
#include "resources/script_resource.hpp"

namespace ts
{
    namespace cup
    {
        struct Car_data
        {
            std::uint16_t car_id;
            Player_handle controller;
            resources::Player_definition player;
            resources::Car_model car_def;
            std::uint32_t start_pos;
        };

        struct Stage_data
        {
            resources::Track_handle track;
            std::vector<resources::Script_handle> script_resources;
            std::vector<Car_data> cars;
        };

    }
}

#endif