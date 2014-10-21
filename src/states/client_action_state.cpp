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
#include "client_action_state.hpp"

#include "client/client.hpp"

ts::states::Client_action_state::Client_action_state(game::Loaded_scene loaded_scene, client::Client* client,
                                                     state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store)
    : Action_state_base(std::move(loaded_scene), client->make_control_interface(), state_machine, context, resource_store),
      client_(client) 
{
    client_->add_cup_listener(this);
}


ts::states::Client_action_state::~Client_action_state()
{
    client_->remove_cup_listener(this);
    client_->clean_stage();
}

void ts::states::Client_action_state::update(std::size_t frame_duration)
{
    Action_state_base::update(frame_duration);

    client_->poll();
    client_->update_stage(frame_duration);
}

void ts::states::Client_action_state::end_action()
{
}