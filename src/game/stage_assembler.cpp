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

#include "cup/cup.hpp"

#include "world/world.hpp"

ts::cup::Stage_data ts::game::assemble_stage(const cup::Cup& cup_obj)
{
    const auto& players = cup_obj.action_players();

    std::uint16_t car_id = 0;

    cup::Stage_data stage_data;
    for (auto player : players)
    {
        // TODO: if player car wasn't cancelled by script

        cup::Car_data car_data;
        car_data.car_id = ++car_id;
        car_data.player = player;
        car_data.color = player->color;
        car_data.start_pos = player->start_pos;
        car_data.car = player->car;
        
        stage_data.cars.push_back(car_data);
    }

    // TODO: add cars as requested by script

    stage_data.track = cup_obj.current_track();
    return stage_data;
}

void ts::game::create_stage_entities(world::World* world, const cup::Stage_data& stage_data)
{
    const auto& start_points = world->track().start_points();

    for (const auto& car_info : stage_data.cars)
    {
        if (car_info.start_pos < start_points.size())
        {
            const auto& start_point = start_points[car_info.start_pos];
            auto car = world->create_car(*car_info.car, car_info.car_id);

            car->set_position(start_point.position);
            car->set_z_position(start_point.level);
            car->set_rotation(start_point.rotation);
        }
    }
}

std::unique_ptr<ts::controls::Control_center> ts::game::create_control_center(const world::World* world, const cup::Stage_data& stage_data)
{
    auto control_center = std::make_unique<controls::Control_center>();

    for (const auto& car_info : stage_data.cars)
    {
        if (car_info.player->control_slot != controls::invalid_slot)
        {
            if (auto car = world->get_car_by_id(car_info.car_id))
            {
                control_center->assume_control(car_info.player->control_slot, car);
            }
        }
    }

    return control_center;
}