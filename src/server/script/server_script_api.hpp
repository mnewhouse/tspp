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

#pragma once

#ifndef SERVER_SCRIPT_API_HPP
#define SERVER_SCRIPT_API_HPP

#include "server/server_messages.hpp"

namespace ts
{
    namespace script
    {
        class Engine;
        class Event_interface;
    }

    namespace resources
    {
        struct Resource_store;
    }

    namespace action
    {
        class Stage_interface;
    }

    namespace server
    {
        class Client_map;
        class Command_center;
        class Stage_assembler;

        namespace script_api
        {
            void register_core_api(script::Engine* engine);

            void register_event_api(script::Engine* engine, const script::Event_interface* event_interface);
            void register_resource_api(script::Engine* engine, const resources::Resource_store* resource_store);
            void register_message_api(script::Engine* engine, const Message_center* message_center);
            void register_client_api(script::Engine* engine, const Client_map* client_map);
            void register_command_api(script::Engine* engine, Command_center* command_center);
            void register_stage_api(script::Engine* engine, const action::Stage_interface* stage_interface);
            void register_stage_assembler_api(script::Engine* engine);

            void register_stdout_console(script::Engine* engine);
        }
    }
}

#endif