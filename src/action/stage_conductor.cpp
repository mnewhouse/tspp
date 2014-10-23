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
#include "stage_conductor.hpp"
#include "stage.hpp"

#include "action/action_messages.hpp"

#include "world/car.hpp"

namespace ts
{
    namespace action
    {
        static bool stage_time_compare(const Control_event_message& event, std::uint32_t stage_time)
        {
            return event.stage_time < stage_time;
        }
    }
}

ts::action::Stage_conductor::Stage_conductor(Stage* stage)
: stage_(stage)
{
}

ts::action::Stage_conductor::~Stage_conductor()
{
}

void ts::action::Stage_conductor::process_control_event_message(const Control_event_message& message)
{
    if (stage_->stage_time() >= message.stage_time)
    {
        handle_control_event_message(message);
    }

    enqueue_control_event_message(message);
}

void ts::action::Stage_conductor::process_game_state_message(const Game_state_message& message)
{
    stage_->set_stage_time(message.stage_time);

    for (const auto& car_state : message.car_states)
    {
        if (auto* car = stage_->get_car_by_id(car_state.car_id))
        {
            car->set_position(car_state.position);
            car->set_rotation(car_state.rotation);
            car->set_velocity(car_state.velocity);
            car->set_z_position(car_state.z_position);
            car->set_current_traction(car_state.traction);
            car->set_angular_velocity(car_state.angular_velocity);
            car->toggle_reversing(car_state.reversing);
            car->set_control_state_mask(car_state.controls_mask);
        }
    }
}

void ts::action::Stage_conductor::enqueue_control_event_message(const Control_event_message& message)
{
    auto insertion_point = std::lower_bound(control_event_queue_.begin(), control_event_queue_.end(), message.stage_time, stage_time_compare);

    control_event_queue_.insert(insertion_point, message);
}

void ts::action::Stage_conductor::handle_control_event_message(const Control_event_message& message)
{
    stage_->handle_control_event(message.controllable_id, message.controls_mask);
}

void ts::action::Stage_conductor::purge_old_events(std::uint32_t time_point)
{
    auto it = std::lower_bound(control_event_queue_.begin(), control_event_queue_.end(), time_point, stage_time_compare);

    control_event_queue_.erase(control_event_queue_.begin(), it);
}

void ts::action::Stage_conductor::poll()
{
    auto stage_time = stage_->stage_time();

    auto it = std::lower_bound(control_event_queue_.begin(), control_event_queue_.end(), stage_time, stage_time_compare);

    // See if there are any events for this exact frame
    for (; it != control_event_queue_.end() && it->stage_time == stage_time; ++it)
    {
        handle_control_event_message(*it);
    }    
}

void ts::action::Stage_conductor::update(std::size_t frame_duration)
{
    poll();

    stage_->update(frame_duration);
}

const ts::action::Stage* ts::action::Stage_conductor::stage() const
{
    return stage_;
}

std::uint32_t ts::action::Stage_conductor::stage_time() const
{
    return stage_->stage_time();
}