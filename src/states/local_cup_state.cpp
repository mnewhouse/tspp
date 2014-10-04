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
#include "local_cup_state.hpp"
#include "local_action_state.hpp"

#include "cup/local_players.hpp"

ts::states::impl::Local_cup_state_members::Local_cup_state_members(resources::Resource_store* resource_store)
: cup_(cup::Cup_type::Local), 
  cup_config_(&cup_, resource_store),
  local_cup_interface_(&cup_)
{
    cup::add_selected_local_players(&local_cup_interface_, resource_store->player_settings(), resource_store->player_store());
    local_cup_interface_.advance();
}

ts::states::Local_cup_state::Local_cup_state(state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store)
: Local_cup_state_members(resource_store),
  Cup_state_base(&local_cup_interface_, state_machine, context, resource_store)
{
    cup_.add_cup_listener(this);
}

ts::states::Local_cup_state::~Local_cup_state()
{
}

void ts::states::Local_cup_state::on_state_change(cup::Cup_state old_state, cup::Cup_state new_state)
{
    Cup_state_base::on_state_change(old_state, new_state);
}

void ts::states::Local_cup_state::update(std::size_t frame_duration)
{
    Cup_state_base::update(frame_duration);
}

std::unique_ptr<ts::states::Action_state_base> ts::states::Local_cup_state::create_action_state(game::Loaded_scene loaded_scene)
{
    return std::make_unique<Local_action_state>(std::move(loaded_scene), state_machine(), context(), resource_store());
}