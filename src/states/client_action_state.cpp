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
#include "client/scene/scene.hpp"

#include "controls/control_interface.hpp"

#include "action/stage.hpp"

struct ts::states::Client_action_state::Members
{
    Generic_scope_exit action_;
};

ts::states::Client_action_state::Client_action_state(client::Client* client, state_machine_type* state_machine, 
                                                     gui::Context* context, resources::Resource_store* resource_store)
: Action_state_base(client->make_control_interface(), client->client_interface(), state_machine, context, resource_store),
  client_(client),
  members_(std::make_unique<Members>())
{    
}

ts::states::Client_action_state::~Client_action_state()
{
}

void ts::states::Client_action_state::update(std::size_t frame_duration)
{
    client_->update(frame_duration);
}

void ts::states::Client_action_state::on_activate()
{
    Action_state_base::on_activate();

    members_->action_ = client_->launch_action();
}

ts::scene::Scene ts::states::Client_action_state::acquire_scene()
{
    return client_->acquire_scene();
}