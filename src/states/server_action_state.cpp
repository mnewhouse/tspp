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

#include "server/server.hpp"
#include "action/stage.hpp"

#include "game/chatbox_display.hpp"

ts::states::Server_action_state::Server_action_state(game::Loaded_scene loaded_scene, server::Server* server, client::Local_client* local_client,
                                                     state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store)
    : Action_state_base(std::move(loaded_scene), 
          std::make_unique<game::Chatbox_display>(*local_client->chatbox(), local_client->client_interface(), context), 
          local_client->make_control_interface(server->stage()), state_machine, context, resource_store),
      server_(server)
{
}

ts::states::Server_action_state::~Server_action_state()
{
    server_->stage()->remove_world_listener(this);

    server_->remove_cup_listener(this);
    server_->end_action();    
}

void ts::states::Server_action_state::update(std::size_t frame_duration)
{
    Action_state_base::update(frame_duration);

    server_->update(frame_duration);
}

void ts::states::Server_action_state::on_activate()
{
    Action_state_base::on_activate();

    server_->add_cup_listener(this);
    server_->launch_action();

    server_->stage()->add_world_listener(this);
}
