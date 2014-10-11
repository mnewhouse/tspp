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

#include "action/server_action_interface.hpp"

ts::states::Server_action_state::Server_action_state(game::Loaded_scene loaded_scene, std::unique_ptr<action::Server_action_interface> action_interface, state_machine_type* state_machine,
                                                     gui::Context* context, resources::Resource_store* resource_store)
    : Action_state_base(std::move(loaded_scene), state_machine, context, resource_store),
      action_interface_(std::move(action_interface))
{
}

ts::states::Server_action_state::~Server_action_state()
{
}

void ts::states::Server_action_state::update(std::size_t frame_duration)
{
}