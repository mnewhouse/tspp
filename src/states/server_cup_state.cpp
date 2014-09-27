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
#include "server_cup_state.hpp"
#include "server_action_state.hpp"

ts::states::Server_cup_state::Server_cup_state(state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store)
: Cup_state_base(game::Cup_type::Server, state_machine, context, resource_store)
{
    auto& network_settings = resource_store->settings.network_settings;
    server_.listen(network_settings.server_port);

    add_selected_local_players();
}

ts::states::Server_cup_state::~Server_cup_state()
{
}

std::unique_ptr<ts::states::Action_state_base> ts::states::Server_cup_state::create_action_state(game::Loaded_scene loaded_scene)
{
    return std::make_unique<Server_action_state>(std::move(loaded_scene), &server_, state_machine(), context(), resource_store());
}

void ts::states::Server_cup_state::on_state_change(game::Cup_state old_state, game::Cup_state new_state)
{
    Cup_state_base::on_state_change(old_state, new_state);
}