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

ts::states::Local_cup_state::Local_cup_state(state_machine_type* state_machine, gui::Context* context,
                                             resources::Resource_store* resource_store)
                                             : Cup_state_base(game::Cup_type::Local, state_machine, context, resource_store)
{
}

ts::states::Local_cup_state::~Local_cup_state()
{
}

std::unique_ptr<ts::states::Action_state_base> ts::states::Local_cup_state::create_action_state(game::Loaded_scene loaded_scene)
{
    return std::make_unique<Local_action_state>(std::move(loaded_scene), state_machine(), context(), resource_store());
}