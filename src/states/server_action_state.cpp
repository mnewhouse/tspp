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

#include "server/server.hpp"

ts::states::Server_action_state::Server_action_state(game::Loaded_scene loaded_scene, server::Server* server, controls::Control_interface* control_interface,
                                                     state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store)
    : Action_state_base(std::move(loaded_scene), control_interface, state_machine, context, resource_store),
      server_(server)
{
    server_->add_cup_listener(this);
}

ts::states::Server_action_state::~Server_action_state()
{
    server_->remove_cup_listener(this);
    server_->clean_stage();
}

void ts::states::Server_action_state::update(std::size_t frame_duration)
{
    Action_state_base::update(frame_duration);

    server_->update(frame_duration);
}

void ts::states::Server_action_state::on_activate()
{
    Action_state_base::on_activate();

    server_->launch_action();
}

void ts::states::Server_action_state::end_action()
{
    server_->end_action();    
}