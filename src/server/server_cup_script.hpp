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

#ifndef SERVER_CUP_SCRIPT_HPP
#define SERVER_CUP_SCRIPT_HPP

#include "server_messages.hpp"

namespace ts
{
    namespace cup
    {
        class Cup_controller;
    }

    namespace server
    {
        class Command_center;
        class Client_map;
        class Stage_assembler;

        class Cup_script_interface
        {
        public:
            Cup_script_interface(Message_center* message_center, Command_center* command_center, 
                                 cup::Cup_controller* cup_controller, Client_map* client_map);
            ~Cup_script_interface();

            void handle_client_connect(const Generic_client& client);
            void handle_client_disconnect(const Generic_client& client);

            void handle_pre_initialization(Stage_assembler* stage_assembler);

        private:
            struct Impl;
            std::unique_ptr<Impl> impl_;
        };
    }
}

#endif