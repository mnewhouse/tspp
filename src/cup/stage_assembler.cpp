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
#include "stage_assembler.hpp"

#include "cup_controller.hpp"
#include "cup.hpp"

ts::cup::Stage_data ts::cup::assemble_stage(const Cup_controller& cup_controller)
{
    Stage_data stage_data;

    const auto& players = cup_controller.stage_players();
    
    std::uint16_t car_id = 0;
    
    for (auto stage_player : players)
    {
        const auto& player = stage_player.player;
        const auto& car = stage_player.car;
        if (player)
        {
            // This can proceed even if there is no car available, because it might be set later.
            // And if it's not set, we have to start without this player.

            stage_data.cars.emplace_back();
            auto& car_data = stage_data.cars.back();

            car_data.car_id = ++car_id;
            car_data.car_def = car;
            car_data.controller = player;
            car_data.player.name = player->nickname;
            car_data.player.id = player->id;
            car_data.player.color = player->color;
            car_data.start_pos = stage_player.start_pos;
        }
    }

    stage_data.track = cup_controller.current_track();
    return stage_data;
}