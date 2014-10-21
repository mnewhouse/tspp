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
#include "client_script_interface.hpp"

#include "script/script_engine.hpp"

#include "script/api_definitions/utility_classes.hpp"
#include "script/api_definitions/world_api.hpp"
#include "script/api_definitions/event_api.hpp"
#include "script/api_definitions/view_api.hpp"



#include "world/world.hpp"

ts::script_api::Client_interface::Client_interface(world::World* world, game::Action_scene* action_scene)
: world_interface_(this)
{
    register_api(script_api::utility_classes());
    register_api(script_api::event_api());
    register_api(script_api::world_api(world));
    register_api(script_api::view_api(action_scene));

    world->add_world_listener(&world_interface_);
}
