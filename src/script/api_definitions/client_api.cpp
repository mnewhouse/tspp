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
#include "client_api.hpp"

#include "script/script_delegates.hpp"
#include "script/argument_stream.hpp"
#include "script/script_utility.hpp"
#include "script/script_module.hpp"

#include "server/client_map.hpp"

namespace ts
{
    namespace script_api
    {
        using namespace script;

        namespace client
        {
            SQInteger getName(HSQUIRRELVM vm);
            SQInteger getIpAddress(HSQUIRRELVM vm);

            const Member_function_definition member_functions[] =
            {
                { "getName", getName },
                { "getIpAddress", getIpAddress }
            };
        }
        
        template <>
        struct Delegate_traits<server::Generic_client>
        {
            static Range<const Member_function_definition*> member_functions();
        };
    }
}

ts::Range<const ts::script_api::Member_function_definition*> ts::script_api::Delegate_traits<ts::server::Generic_client>::member_functions()
{
    return make_range(std::begin(client::member_functions), std::end(client::member_functions));
}

ts::script::API_definition ts::script_api::client_api(const server::Client_map* client_map)
{
    API_definition result;
    result.interfaces.push_back(make_interface(client_map));
    result.delegates.push_back(create_delegate_definition<server::Generic_client>());

    return result;
}

SQInteger ts::script_api::client::getName(HSQUIRRELVM vm)
{   
    Userdata<server::Generic_client> client_udata;

    Argument_stream argument_stream(vm, "Client::getName");
    argument_stream(Userdata_reader<server::Generic_client>(client_udata));

    if (argument_stream)
    {
        auto client_map = get_interface<const server::Client_map>(vm);
        auto name = client_map->client_name(*client_udata);
        sq_pushstring(vm, name.data(), name.size());
        return 1;
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
        return 0;
    }
}


SQInteger ts::script_api::client::getIpAddress(HSQUIRRELVM vm)
{   
    Userdata<server::Generic_client> client_udata;

    Argument_stream argument_stream(vm, "Client::getIpAddress");
    argument_stream(Userdata_reader<server::Generic_client>(client_udata));

    if (argument_stream)
    {
        auto client_map = get_interface<const server::Client_map>(vm);
        auto ip_address = client_map->client_address(*client_udata).toString();
        sq_pushstring(vm, ip_address.data(), ip_address.size());
        return 1;
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
        return 0;
    }
}