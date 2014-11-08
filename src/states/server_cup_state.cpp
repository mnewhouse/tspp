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

#include "cup/stage_assembler.hpp"
#include "cup/cup.hpp"

#include "resources/settings/network_settings.hpp"
#include "resources/settings/player_settings.hpp"
#include "resources/player_store.hpp"

#include "client/client_interface.hpp"

namespace ts
{
    namespace action
    {
        class Stage;
    }

    namespace states
    {
        std::vector<cup::Player_definition> create_local_player_definitions(const resources::Player_settings& player_settings, const resources::Player_store& player_store);
    }
}

ts::states::impl::Server_cup_state_members::Server_cup_state_members(resources::Resource_store* resource_store)
: server_(resource_store),
  local_client_(&server_, resource_store)
{
}

ts::states::Server_cup_state::Server_cup_state(state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store)
  : Server_cup_state_members(resource_store),
    Cup_state_base(local_client_.client_interface(), state_machine, context, resource_store)
{
    server_.add_cup_listener(this);
}

ts::states::Server_cup_state::~Server_cup_state()
{
    server_.remove_cup_listener(this);
}

void ts::states::Server_cup_state::update(std::size_t frame_duration)
{
    Cup_state_base::update(frame_duration);

    server_.update(frame_duration);
}

void ts::states::Server_cup_state::listen(std::uint16_t port)
{
    server_.listen(port);
}

void ts::states::Server_cup_state::on_activate()
{
    Cup_state_base::on_activate();

    if (server_.cup()->cup_state() == cup::Cup_state::Action)
    {
        local_client_.client_interface()->request_advance();
    }
}

void ts::states::Server_cup_state::on_initialize(const cup::Stage_data& stage_data)
{
    auto stage_loader = server_.async_load_stage(stage_data, [this](const action::Stage* stage)
    {
        load_scene(stage);
    });

    show_stage_loading(stage_loader);
}

std::unique_ptr<ts::states::Action_state_base> ts::states::Server_cup_state::make_action_state(game::Loaded_scene loaded_scene)
{
    return std::make_unique<Server_action_state>(std::move(loaded_scene), &server_, &local_client_,
                                                 state_machine(), context(), resource_store());
}

