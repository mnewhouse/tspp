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
#include "color_api.hpp"

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

        namespace chat_message
        {
            SQInteger append(HSQUIRRELVM vm);
            SQInteger constructor(HSQUIRRELVM vm);

            const Member_function_definition member_functions[] =
            {
                { "append", append },
                { "constructor", constructor }
            };
        }

        
        struct Chat_message_reader
        {
            Chat_message_reader(cup::Chat_message& result);

            bool operator()(HSQUIRRELVM vm, SQInteger index) const;

        private:
            cup::Chat_message& result_;
        };

        struct Chat_message_component_reader
        {
            Chat_message_component_reader(cup::Chat_message_component& result);

            bool operator()(HSQUIRRELVM vm, SQInteger index) const;

        private:
            cup::Chat_message_component& result_;
        };
    }
}

ts::script::API_definition ts::script_api::server_chatbox_api(server::Message_center* message_center)
{
    API_definition result;
    result.interfaces.push_back(make_interface(message_center));
    result.static_functions.assign(std::begin(chatbox_server::static_functions), std::end(chatbox_server::static_functions));

    Class_definition chat_message_definition;
    chat_message_definition.class_name = classes::ChatMessage;
    chat_message_definition.add_member(members::chat_message::components);
    chat_message_definition.member_functions.assign(std::begin(chat_message::member_functions), std::end(chat_message::member_functions));
    result.classes.push_back(chat_message_definition);

    Class_definition chat_message_component_definition;
    chat_message_component_definition.class_name = classes::ChatMessageComponent;
    chat_message_component_definition.add_member(members::chat_message_component::sub_string);
    chat_message_component_definition.add_member(members::chat_message_component::color);
    result.classes.push_back(chat_message_component_definition);

    return result;
}

SQInteger ts::script_api::chatbox_server::outputChatMessage(HSQUIRRELVM vm)
{
    cup::Chat_message chat_message;
    Userdata<server::Generic_client> client_udata;

    Argument_stream argument_stream(vm, "outputChatMessage");
    argument_stream(ignore_argument);
    argument_stream(Chat_message_reader(chat_message));
    argument_stream(Userdata_reader<server::Generic_client>(client_udata), arg::optional);

    if (argument_stream)
    {
        auto message_center = get_interface<server::Message_center>(vm);
        
        server::Client_message message;
        message.message = cup::make_chatbox_output_message(chat_message);
        message.message_type = server::Message_type::Reliable;

        if (client_udata)
        {
            message.client = *client_udata;
        }

        message_center->dispatch_message(message);
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
    }

    return 0;
}


SQInteger ts::script_api::chat_message::constructor(HSQUIRRELVM vm)
{
    Object_handle instance, color;
    utf8_string_view string;

    Stack_guard stack_guard(vm);

    Argument_stream argument_stream(vm, "ChatMessage::constructor");
    argument_stream(Instance_reader(classes::ChatMessage, instance));
    argument_stream(Tostring_reader(string), arg::optional);
    argument_stream(Instance_reader(classes::Color, color), arg::optional);

    if (argument_stream)
    {
        auto component_class = get_class_by_name(vm, classes::ChatMessageComponent);
        auto color_class = get_class_by_name(vm, classes::Color);
        component_class.push();
        color_class.push();

        sq_createinstance(vm, -2);
        sq_pushstring(vm, members::chat_message_component::sub_string, -1);
        sq_pushstring(vm, string.data(), string.size());
        sq_set(vm, -3);

        sq_pushstring(vm, members::chat_message_component::color, -1);
        if (color)
        {            
            color.push();
        }

        else
        {
            // Construct new color instance
            sq_createinstance(vm, -3);

            Stack_guard call_guard(vm);
            sq_pushstring(vm, "constructor", -1);
            if (SQ_SUCCEEDED(sq_get(vm, -5)))
            {
                // And call the constructor.
                sq_push(vm, -2);
                sq_pushinteger(vm, 255);
                sq_pushinteger(vm, 255);
                sq_pushinteger(vm, 255);
                sq_pushinteger(vm, 255);
                sq_call(vm, 5, SQFalse, SQTrue);
            }
        }

        sq_set(vm, -3);        

        sq_newarray(vm, 0);
        sq_push(vm, -2);
        sq_arrayappend(vm, -2);

        instance.push();
        sq_pushstring(vm, members::chat_message::components, -1);
        sq_push(vm, -3);
        sq_set(vm, -3);       
    }
    
    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
    }

    return 0;
}

SQInteger ts::script_api::chat_message::append(HSQUIRRELVM vm)
{
    Object_handle object, color;
    utf8_string_view sub_string;

    Argument_stream argument_stream(vm, "ChatMessage::append");
    argument_stream(Instance_reader(classes::ChatMessage, object));
    argument_stream(Tostring_reader(sub_string));
    argument_stream(Instance_reader(classes::Color, color), arg::optional);

    if (argument_stream)
    {
        auto component_class = get_class_by_name(vm, classes::ChatMessageComponent);
        component_class.push();

        sq_createinstance(vm, -1);
        sq_pushstring(vm, members::chat_message_component::sub_string, -1);
        sq_pushstring(vm, sub_string.data(), sub_string.size());
        sq_set(vm, -3);

        sq_pushstring(vm, members::chat_message_component::color, -1);
        color.push();
        sq_set(vm, -3);

        object.push();
        sq_pushstring(vm, members::chat_message::components, -1);

        if (SQ_SUCCEEDED(sq_get(vm, -2)))
        {
            if (sq_gettype(vm, -1) != OT_ARRAY)
            {
                sq_pop(vm, 1);
                sq_newarray(vm, 0);
            }

            sq_push(vm, -3);
            sq_arrayappend(vm, -2);
            sq_pushstring(vm, members::chat_message::components, -1);
            sq_push(vm, -2);
            sq_set(vm, -4);
        }        
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
    }

    object.push();
    return 1;
}

ts::script_api::Chat_message_component_reader::Chat_message_component_reader(cup::Chat_message_component& result)
: result_(result)
{
}

bool ts::script_api::Chat_message_component_reader::operator()(HSQUIRRELVM vm, SQInteger index) const
{
    Object_handle component_instance;
    Stack_guard stack_guard(vm);

    if (Instance_reader(classes::ChatMessageComponent, component_instance)(vm, index))
    {
        component_instance.push();
        sq_pushstring(vm, members::chat_message_component::sub_string, -1);

        utf8_string_view sub_string;
        sf::Color color;

        if (SQ_SUCCEEDED(sq_get(vm, -2)) && Tostring_reader(sub_string)(vm, -1))
        {
            component_instance.push();
            sq_pushstring(vm, members::chat_message_component::color, -1);
            if (SQ_SUCCEEDED(sq_get(vm, -2)) && Color_reader(color)(vm, -1))
            {
                result_.text = sub_string;
                result_.color = color;
                return true;
            }
        }
    }

    return false;
}

ts::script_api::Chat_message_reader::Chat_message_reader(cup::Chat_message& result)
: result_(result)
{
}

bool ts::script_api::Chat_message_reader::operator()(HSQUIRRELVM vm, SQInteger index) const
{
    utf8_string_view string_result;
    if (String_reader(string_result)(vm, index))
    {
        result_ = utf8_string(string_result);
        return true;
    }

    Object_handle chat_message;
    if (Instance_reader(classes::ChatMessage, chat_message)(vm, index))
    {
        Stack_guard stack_guard(vm);
        chat_message.push();
        sq_pushstring(vm, members::chat_message::components, -1);
        if (SQ_SUCCEEDED(sq_get(vm, -2)))
        {
            result_ = {};
            if (sq_gettype(vm, -1) != OT_ARRAY)
            {
                // Return an empty message
                return true;
            }

            SQInteger num_components = sq_getsize(vm, -1);
            for (SQInteger component_index = 0; component_index < num_components; ++component_index)
            {
                Stack_guard loop_guard(vm);
                sq_pushinteger(vm, component_index);
                sq_get(vm, -2);

                cup::Chat_message_component component;

                if (Chat_message_component_reader(component)(vm, -1))
                {
                    result_.append(component);
                }
            }

            return true;
        }        
    }

    return false;
}