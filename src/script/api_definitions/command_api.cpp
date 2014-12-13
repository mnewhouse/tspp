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
#include "command_api.hpp"

#include "server/command_center.hpp"
#include "server/generic_client.hpp"

#include "script/argument_stream.hpp"
#include "script/script_utility.hpp"
#include "script/script_module.hpp"
#include "script/script_userdata.hpp"
#include "script/script_function.hpp"

namespace ts
{
	namespace script
	{
		template <>
		struct Userdata_traits<server::Command_center::Command_handle>
			: script::Default_userdata_traits
		{
			static const bool copyable = false;
			static const char* const type_name;
		};

		const char* const Userdata_traits<server::Command_center::Command_handle>::type_name = "CommandHandle";
	}

    namespace script_api
    {
        using namespace script;

        using Command_handle = server::Command_center::Command_handle;


        namespace commands
        {
            SQInteger addCommandHandler(HSQUIRRELVM vm);
            SQInteger removeCommandHandler(HSQUIRRELVM vm);

            const Static_function_definition static_functions[] =
            {
                { "addCommandHandler", addCommandHandler },
                { "removeCommandHandler", removeCommandHandler }
            };
        }

        struct Command_handler_function
        {
        public:
            Command_handler_function(HSQUIRRELVM vm, script::Function function);

            void operator()(const server::Generic_client& client, const utf8_string& command_name, const utf8_string& arguments) const;

        private:
            Function function_;
            HSQUIRRELVM vm_;
        };

        void store_command_handler(HSQUIRRELVM vm, const Userdata<Command_handle>& userdata)
        {
            static int storage_key = 0;

            Stack_guard stack_guard(vm);

            sq_pushconsttable(vm);
            sq_pushuserpointer(vm, static_cast<SQUserPointer>(&storage_key));
            if (SQ_FAILED(sq_get(vm, -2)))
            {
                sq_newtable(vm);
                sq_push(vm, -1);
                sq_newslot(vm, -3, SQFalse);
            }

            userdata.push();
            userdata.push();
            sq_newslot(vm, -3, SQFalse);            
        }
    }
}

ts::script_api::Command_handler_function::Command_handler_function(HSQUIRRELVM vm, script::Function function)
: function_(function),
  vm_(vm)
{
}

void ts::script_api::Command_handler_function::operator()(const server::Generic_client& client, const utf8_string& command_name,
                                                          const utf8_string& arguments) const
{
    function_(script::get_root_table(vm_), make_userdata(vm_, client), command_name, arguments);
}

ts::script::API_definition ts::script_api::command_api(server::Command_center* command_center)
{
    API_definition result;
    result.static_functions.assign(std::begin(commands::static_functions), std::end(commands::static_functions));
    result.interfaces.push_back(make_interface(command_center));
    
    return result;
}

SQInteger ts::script_api::commands::addCommandHandler(HSQUIRRELVM vm)
{
    utf8_string_view command_name;
    Function handler_function;

    Argument_stream argument_stream(vm, "addCommandHandler");
    argument_stream(ignore_argument);
    argument_stream(String_reader(command_name));
    argument_stream(Function_reader(handler_function));
    
    if (argument_stream)
    {
        auto command_center = get_interface<server::Command_center>(vm);
        auto command_handler = command_center->add_command_handler(command_name, Command_handler_function(vm, handler_function));
        auto result = make_userdata(vm, std::move(command_handler));

        store_command_handler(vm, result);
        result.push();

        return 1;
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
        return 0;
    }
}

SQInteger ts::script_api::commands::removeCommandHandler(HSQUIRRELVM vm)
{
    Userdata<Command_handle> command_udata;

    Argument_stream argument_stream(vm, "removeCommandHandler");
    argument_stream(ignore_argument);
    argument_stream(Userdata_reader<Command_handle>(command_udata));

    if (argument_stream)
    {
        if (*command_udata)
        {
            auto command_center = get_interface<server::Command_center>(vm);
            command_center->remove_command_handler(std::move(*command_udata));
        }

        else
        {
            // Report error.
        }
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
    }

    return 0;
}