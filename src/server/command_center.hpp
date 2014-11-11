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

#ifndef COMMAND_MANAGER_HPP
#define COMMAND_MANAGER_HPP

#include "command_handler.hpp"

namespace ts
{
    namespace server
    {
        class Generic_client;

        class Command_center
        {
        public:
            struct Command_handle;

            Command_handle add_command_handler(const utf8_string& command_name, Command_handler handler);
            void remove_command_handler(Command_handle command_handle);

            void handle_client_message(const Generic_client& client, const utf8_string& message) const;
            bool has_command_prefix(const utf8_string& string) const;

        private:
            std::map<utf8_string, std::list<Command_handler>> command_map_;
        };

        struct Command_center::Command_handle
        {
        public:
            Command_handle();
            ~Command_handle();

            Command_handle(const Command_handle& other) = delete;
            Command_handle& operator=(const Command_handle& other) = delete;

            Command_handle(Command_handle&& other);
            Command_handle& operator=(Command_handle&& other);            

            explicit operator bool() const;
            const utf8_string& command_name() const;

        private:
            friend class Command_center;
            Command_handle(Command_center* command_center, const utf8_string* command_name, std::list<Command_handler>::iterator it);

            Command_center* command_center_ = nullptr;
            const utf8_string* command_name_ = nullptr;
            std::list<Command_handler>::iterator it_;
        };
    }
}

#endif