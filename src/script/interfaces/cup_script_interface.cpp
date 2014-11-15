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
#include "cup_script_interface.hpp"

#include "resources/resource_store.hpp"

#include "script/api_definitions/event_api.hpp"
#include "script/api_definitions/utility_classes.hpp"
#include "script/api_definitions/color_api.hpp"
#include "script/api_definitions/player_color_api.hpp"
#include "script/api_definitions/command_api.hpp"
#include "script/api_definitions/chatbox_api.hpp"
#include "script/api_definitions/client_api.hpp"
#include "script/api_definitions/car_store_api.hpp"
#include "script/api_definitions/stage_assembler_api.hpp"

ts::script_api::Cup_interface::Cup_interface(server::Message_center* message_center, server::Command_center* command_center, 
                                             server::Client_map* client_map, const resources::Resource_store* resource_store)
{
    register_api(script_api::utility_classes());
    register_api(script_api::event_api());
    register_api(script_api::color_api());
    register_api(script_api::player_color_api());

    register_api(script_api::client_api(client_map));
    register_api(script_api::command_api(command_center));
    register_api(script_api::server_chatbox_api(message_center));    
    register_api(script_api::stage_assembler_api());
    register_api(script_api::car_store_api(&resource_store->car_store()));
}
