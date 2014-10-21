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
#include "stage.hpp"

#include "world/world.hpp"
#include "controls/control_center.hpp"

ts::action::Stage::Stage(std::unique_ptr<world::World> world, const Stage_data& stage_data)
: world_(std::move(world)),
  control_center_(std::make_unique<controls::Control_center>()),
  track_handle_(track_handle_)
{
    create_stage_entities(stage_data);
}

ts::action::Stage::~Stage()
{
}

void ts::action::Stage::update(std::size_t frame_duration)
{
    if (is_started_)
    {
        world_->update(frame_duration);
        stage_time_ += frame_duration;
    }    
}

std::uint32_t ts::action::Stage::stage_time() const
{
    return stage_time_;
}

void ts::action::Stage::create_stage_entities(const Stage_data& stage_data)
{
    const auto& track = world_->track();
    const auto& start_points = track.start_points();

    for (const auto& car_data : stage_data.cars)
    {
        if (car_data.start_pos < start_points.size() && car_lookup_.find(car_data.car_id) == car_lookup_.end())
        {
            const auto& start_point = start_points[car_data.start_pos];

            auto car = world_->create_car(*car_data.car_def);
            car->set_position(start_point.position);
            car->set_rotation(start_point.rotation);
            car->set_z_position(start_point.level);

            car_data_.emplace_back();
            car_data_.back().car = car;

            Car_data& internal = car_data_.back();
            internal = car_data;

            if (car_data.controller && car_data.controller->control_slot != controls::invalid_slot)
            {
                control_center_->assume_control(car_data.controller->control_slot, car);
            }

            car_lookup_[car_data.car_id] = car;
            car->set_controllable_id(car_data.car_id);
        }
    }
}

const std::vector<ts::action::Stage::Internal_car_data>& ts::action::Stage::car_data() const
{
    return car_data_;
}

const ts::world::World& ts::action::Stage::world() const
{
    return *world_;
}

const ts::controls::Control_center& ts::action::Stage::control_center() const
{
    return *control_center_;
}

void ts::action::Stage::handle_control_event(const controls::Control_event& event) const
{
    control_center_->handle_control_event(event);
}

void ts::action::Stage::launch_game()
{
    world_->launch_game();

    is_started_ = true;
}