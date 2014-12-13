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
#include "server_action_state.hpp"

#include "client/local_client.hpp"
#include "client/scene/scene.hpp"

#include "controls/control_interface.hpp"

#include "server/server.hpp"
#include "action/stage.hpp"

// Make sure the stage is freed in an orderly fashion.
struct ts::states::Server_action_state::Members
{
    // Client action depends on server action, so client needs to be destroyed first.
    Generic_scope_exit server_action_;
    Generic_scope_exit client_action_;    
};

ts::states::Server_action_state::Server_action_state(server::Server* server, client::Local_client* local_client,
    state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store)
    : Action_state_base(local_client->make_control_interface(server->stage()), local_client->client_interface(),
                        state_machine, context, resource_store),
      server_(server),
      local_client_(local_client),
      members_(std::make_unique<Members>())
{
}

ts::states::Server_action_state::~Server_action_state()
{
}

void ts::states::Server_action_state::update(std::size_t frame_duration)
{
    Action_state_base::update(frame_duration);

    local_client_->update(frame_duration);
    server_->update(frame_duration);   
}

void ts::states::Server_action_state::on_activate()
{
    Action_state_base::on_activate();

    members_->server_action_ = server_->launch_action();
}

ts::scene::Scene ts::states::Server_action_state::acquire_scene()
{
    return local_client_->acquire_scene();
}