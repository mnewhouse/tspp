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
#include "chatbox_api.hpp"

#include "script/argument_stream.hpp"
#include "script/script_utility.hpp"
#include "script/script_module.hpp"

#include "cup/cup_messages.hpp"

namespace ts
{
    namespace script_api
    {
        using namespace script;

        namespace chatbox_server
        {
            SQInteger outputChatMessage(HSQUIRRELVM vm);

            const Static_function_definition static_functions[] =
            {
                { "outputChatMessage", outputChatMessage }
            };
        }
    }
}

ts::script::API_definition ts::script_api::server_chatbox_api(server::Message_center* message_center)
{
    API_definition result;
    result.interfaces.push_back(make_interface(message_center));
    result.static_functions.assign(std::begin(chatbox_server::static_functions), std::end(chatbox_server::static_functions));

    return result;
}

SQInteger ts::script_api::chatbox_server::outputChatMessage(HSQUIRRELVM vm)
{
    utf8_string_view chat_message;

    Argument_stream argument_stream(vm);
    argument_stream(ignore_argument);
    argument_stream(String_reader(chat_message));

    if (argument_stream)
    {
        auto message_center = get_interface<server::Message_center>(vm);
        
        server::Client_message message;
        message.message = cup::make_chatbox_output_message(utf8_string(chat_message));
        message.message_type = server::Message_type::Reliable;

        message_center->dispatch_message(message);
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
    }

    return 0;
}