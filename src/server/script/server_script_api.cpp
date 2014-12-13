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
#include "server_script_api.hpp"
#include "server/command_center.hpp"
#include "server/stage_assembler.hpp"
#include "server/client_map.hpp"
#include "resources/resource_store.hpp"

#include "script/script_engine.hpp"
#include "script/script_console.hpp"

#include "script/api_definitions/event_api.hpp"
#include "script/api_definitions/utility_classes.hpp"
#include "script/api_definitions/player_color_api.hpp"
#include "script/api_definitions/color_api.hpp"

#include "script/api_definitions/client_api.hpp"
#include "script/api_definitions/command_api.hpp"
#include "script/api_definitions/chatbox_api.hpp"
#include "script/api_definitions/stage_api.hpp"
#include "script/api_definitions/stage_assembler_api.hpp"
#include "script/api_definitions/car_store_api.hpp"

namespace ts
{
    namespace server
    {
        namespace script_api
        {
            using namespace ts::script_api;
        }
    }
}

void ts::server::script_api::register_core_api(script::Engine* engine)
{
    engine->register_api(script_api::utility_classes());
    engine->register_api(script_api::player_color_api());
    engine->register_api(script_api::color_api());
}

void ts::server::script_api::register_client_api(script::Engine* engine, const Client_map* client_map)
{
    engine->register_api(script_api::client_api(client_map));
}

void ts::server::script_api::register_command_api(script::Engine* engine, Command_center* command_center)
{
    engine->register_api(script_api::command_api(command_center));
}

void ts::server::script_api::register_event_api(script::Engine* engine, const script::Event_interface* event_interface)
{
    engine->register_api(script_api::event_api(event_interface));
}

void ts::server::script_api::register_resource_api(script::Engine* engine, const resources::Resource_store* resource_store)
{
    engine->register_api(script_api::car_store_api(&resource_store->car_store()));
}

void ts::server::script_api::register_message_api(script::Engine* engine, const Message_center* message_center)
{
    engine->register_api(script_api::server_chatbox_api(message_center));
}

void ts::server::script_api::register_stage_api(script::Engine* engine, const action::Stage_interface* stage_interface)
{
    engine->register_api(script_api::stage_api(stage_interface));
}

void ts::server::script_api::register_stage_assembler_api(script::Engine* engine)
{
    engine->register_api(script_api::stage_assembler_api());
}

void ts::server::script_api::register_stdout_console(script::Engine* engine)
{
    engine->register_console(script::Stdout_console());
}