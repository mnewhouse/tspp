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

#include "cup/local_players.hpp"
#include "cup/stage_assembler.hpp"

#include "resources/settings/network_settings.hpp"

#include "action/server_action_interface.hpp"
#include "network/server_message_source.hpp"

ts::states::impl::Server_cup_state_members::Server_cup_state_members(resources::Resource_store* resource_store)
: cup_(cup::Cup_type::Server),
  cup_config_(&cup_, resource_store),
  server_(),
  message_center_(network::Server_message_source(&server_)),
  server_cup_interface_(&cup_, &server_, resource_store)  
{
    cup::add_selected_local_players(&server_cup_interface_, resource_store->player_settings(), resource_store->player_store());
}

ts::states::Server_cup_state::Server_cup_state(state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store)
: Server_cup_state_members(resource_store),
  Cup_state_base(&server_cup_interface_, state_machine, context, resource_store)
{
    auto& network_settings = resource_store->network_settings();
    server_.listen(network_settings.server_port);

    cup_.add_cup_listener(this);
    cup_.set_cup_state(cup::Cup_state::Registering);
}

ts::states::Server_cup_state::~Server_cup_state()
{    
}

void ts::states::Server_cup_state::update(std::size_t frame_duration)
{
    Cup_state_base::update(frame_duration);
}

void ts::states::Server_cup_state::on_state_change(cup::Cup_state old_state, cup::Cup_state new_state)
{
    Cup_state_base::on_state_change(old_state, new_state);

    if (new_state == cup::Cup_state::Initializing)
    {
        start_loading();
    }

    if (new_state == cup::Cup_state::Action)
    {
        launch_action(make_action_state());
    }
}

void ts::states::Server_cup_state::start_loading()
{
    auto stage_data = cup::assemble_stage(cup_);

    server_cup_interface_.send_initialization_message(stage_data);

    begin_loading_sequence(stage_data);
}


std::unique_ptr<ts::states::Server_action_state> ts::states::Server_cup_state::make_action_state()
{
    auto loaded_scene = transfer_loaded_scene();

    auto action_interface = std::make_unique<action::Server_action_interface>(loaded_scene.stage.get(), server_cup_interface_.client_player_mapping(),
                                                                              &message_center_);

    return std::make_unique<Server_action_state>(std::move(loaded_scene), std::move(action_interface),
                                                 state_machine(), context(), resource_store());
}