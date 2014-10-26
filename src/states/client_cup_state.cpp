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
#include "client_cup_state.hpp"
#include "client_action_state.hpp"

#include "client/client_interface.hpp"

ts::states::impl::Client_cup_state_members::Client_cup_state_members(resources::Resource_store* resource_store)
: client_(resource_store)
{}

ts::states::Client_cup_state::Client_cup_state(state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store)
: Client_cup_state_members(resource_store),
  Cup_state_base(client_.client_interface(), state_machine, context, resource_store)
{
    client_.add_cup_listener(this);
}

ts::states::Client_cup_state::~Client_cup_state()
{
    client_.remove_cup_listener(this);
}

void ts::states::Client_cup_state::update(std::size_t frame_duration)
{
    Cup_state_base::update(frame_duration);

    client_.update(frame_duration);
}

void ts::states::Client_cup_state::on_initialize(const cup::Stage_data& stage_data)
{
    auto stage_loader = client_.async_load_stage(stage_data, [this](const action::Stage* stage)
    {
        load_scene(stage);
    });

    show_stage_loading(stage_loader);
}

void ts::states::Client_cup_state::async_connect(utf8_string remote_address, std::uint16_t remote_port)
{
    client_.async_connect(remote_address, remote_port);
}

void ts::states::Client_cup_state::send_registration_request()
{ 
    client_.send_registration_request();
}

ts::client::Registration_status ts::states::Client_cup_state::registration_status() const
{
    return client_.registration_status();
}

const ts::utf8_string& ts::states::Client_cup_state::registration_error() const
{
    return client_.registration_error();
}

ts::client::Connection_status ts::states::Client_cup_state::connection_status() const
{
    return client_.connection_status();
}

std::unique_ptr<ts::states::Action_state_base> ts::states::Client_cup_state::make_action_state(game::Loaded_scene loaded_scene)
{
    return std::make_unique<ts::states::Client_action_state>(std::move(loaded_scene), &client_,                                                             state_machine(), context(), resource_store());
}