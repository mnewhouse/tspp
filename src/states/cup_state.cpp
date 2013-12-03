/*
 * Turbo Sliders++
 * Copyright (C) 2013 Martin Newhouse
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

#include "cup_state.hpp"
#include "action_state.hpp"

#include "core/utility.hpp"
#include "resources/track.hpp"


ts::states::Cup_state::Cup_state(const Handle<state_machine_type>& state_machine,
                                 const Handle<gui::Context>& context)
                                 : gui::State(state_machine, context)
{
}

void ts::states::Cup_state::async_load()
{
    auto track_handle = current_track();
    auto state_machine_handle = state_machine();
    auto context_handle = context();
    
    make_stage_data();
    auto stg_data = stage_data();

    auto loader = [=]()
    {
        resources::Track track(track_handle);

        return make_unique<Action_state>(std::move(track), stg_data, state_machine_handle, context_handle);
    };

    future_state_ = std::async(std::launch::async, loader);
}

bool ts::states::Cup_state::is_loading() const
{
    return future_state_.valid();
}

bool ts::states::Cup_state::is_loading_finished() const
{
    auto status = future_state_.wait_for(std::chrono::seconds(0));
    return status == std::future_status::ready;
}

void ts::states::Cup_state::launch_action()
{
    state_machine()->change_state(future_state_.get());

    next_track();
}