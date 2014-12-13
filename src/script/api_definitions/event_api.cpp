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
#include "event_api.hpp"

#include "script/event_interface.hpp"

#include "script/argument_stream.hpp"
#include "script/script_utility.hpp"
#include "script/script_module.hpp"
#include "script/script_userdata.hpp"
#include "script/script_delegates.hpp"

namespace ts
{
    namespace script_api
    {
        using namespace script;

        static SQInteger addEventHandler(HSQUIRRELVM vm);
        static SQInteger removeEventHandler(HSQUIRRELVM vm);        
        static SQInteger triggerEvent(HSQUIRRELVM vm);

        namespace event_handler
        {
            static SQInteger remove(HSQUIRRELVM vm);
        }

        const Static_function_definition static_functions[] =
        {
            { "addEventHandler", addEventHandler },
            { "removeEventHandler", removeEventHandler },
            { "triggerEvent", triggerEvent }
        };

        const Member_function_definition event_handler_member_functions[] =
        {
            { "remove", event_handler::remove }
        };

        template <>
        struct Delegate_traits<Event_handler>
        {
            static Range<const Member_function_definition*> member_functions();
        };
    }
}


ts::Range<const ts::script::Member_function_definition*> ts::script_api::Delegate_traits<ts::script::Event_handler>::member_functions()
{
    using script_api::event_handler_member_functions;

    return Range<const Member_function_definition*>(std::begin(event_handler_member_functions), std::end(event_handler_member_functions));
}

ts::script::API_definition ts::script_api::event_api(const script::Event_interface* event_interface)
{
    API_definition result;
    result.interfaces.push_back(make_interface(event_interface));
    result.static_functions.assign(std::begin(static_functions), std::end(static_functions));
    result.delegates.push_back(create_delegate_definition<Event_handler>());

    return result;
}

SQInteger ts::script_api::addEventHandler(HSQUIRRELVM vm)
{
    utf8_string_view event_name;
    Function callback;

    Argument_stream argument_stream(vm, "addEventHandler");
    argument_stream(ignore_argument);
    argument_stream(String_reader(event_name));
    argument_stream(Function_reader(callback));

    auto module = get_module_by_vm(vm);

    if (argument_stream)
    {        
        auto event_handler = module->add_event_handler(event_name, callback);
        auto result = make_userdata(vm, event_handler);
        result.push();
        return 1;
    }

    else
    {
        report_argument_errors(module, argument_stream);
    }    

    return 0;
}

SQInteger ts::script_api::event_handler::remove(HSQUIRRELVM vm)
{
    Userdata<Event_handler> event_handler;
    Argument_stream argument_stream(vm, "EventHandler::remove");
    argument_stream(Userdata_reader<Event_handler>(event_handler));

    auto module = get_module_by_vm(vm);
    if (argument_stream)
    {
        module->remove_event_handler(*event_handler);
    }

    else
    {
        report_argument_errors(module, argument_stream);
    }

    return 0;
}

SQInteger ts::script_api::removeEventHandler(HSQUIRRELVM vm)
{
    Userdata<Event_handler> event_handler;
    Argument_stream argument_stream(vm, "removeEventHandler");
    argument_stream(ignore_argument);
    argument_stream(Userdata_reader<Event_handler>(event_handler));

    auto module = get_module_by_vm(vm);
    if (argument_stream)
    {
        module->remove_event_handler(*event_handler);
    }

    else
    {
        report_argument_errors(module, argument_stream);
    }

    return 0;
}

SQInteger ts::script_api::triggerEvent(HSQUIRRELVM vm)
{
    utf8_string_view event_name;
    Value source;
    std::vector<Value> argument_list;

    Argument_stream argument_stream(vm, "triggerEvent");
    argument_stream(ignore_argument);
    argument_stream(String_reader(event_name));
    argument_stream(Value_reader(source));

    for (Value value; argument_stream(Value_reader(value), arg::optional); )
    {
        argument_list.push_back(value);
    }

    if (argument_stream)
    {   
        auto event_interface = get_interface<const Event_interface>(vm);
        event_interface->trigger_event(event_name, source, argument_list);
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
    }

    return 0;
}