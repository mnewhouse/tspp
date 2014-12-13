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

#ifndef CHATBOX_API_HPP
#define CHATBOX_API_HPP

#include "script/script_api.hpp"

#include "server/server_messages.hpp"

namespace ts
{
    namespace server
    {
        class Command_center;
    }

    namespace script_api
    {
        script::API_definition server_chatbox_api(const server::Message_center* message_center);

        namespace classes
        {
            static const char ChatMessage[] = "ChatMessage";
            static const char ChatMessageComponent[] = "ChatMessageComponent";
        }

        namespace members
        {
            namespace chat_message
            {
                static const char components[] = "components";
            }

            namespace chat_message_component
            {
                static const char color[] = "color";
                static const char sub_string[] = "sub_string";
            }
        }
    }
}

#endif