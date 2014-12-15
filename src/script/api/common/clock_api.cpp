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
#include "clock_api.hpp"

#include "script/argument_stream.hpp"
#include "script/script_delegates.hpp"

namespace ts
{
    namespace script_api
    {
        using namespace script;

        struct Clock
        {
            Clock()
                : start_time(std::chrono::high_resolution_clock::now())
            {
            }

            std::chrono::high_resolution_clock::time_point start_time;

            static SQInteger create(HSQUIRRELVM vm);
            static SQInteger getElapsedTime(HSQUIRRELVM vm);
            static SQInteger restart(HSQUIRRELVM vm);
        };

        static const Member_function_definition clock_member_functions[] =
        {
            { "getElapsedTime", Clock::getElapsedTime },
            { "restart", Clock::restart }
        };

        static const Static_function_definition static_clock_functions[] =
        {
            { "Clock", Clock::create }
        };

        template <>
        struct Delegate_traits<Clock>
        {
            static Range<const Member_function_definition*> member_functions()
            {
                return make_range(std::begin(clock_member_functions), std::end(clock_member_functions));
            }
        };
    }
}

ts::script::API_definition ts::script_api::clock_api()
{
    API_definition result;
    result.delegates.push_back(create_delegate_definition<Clock>());
    result.static_functions.assign(std::begin(static_clock_functions), std::end(static_clock_functions));

    return result;
}

SQInteger ts::script_api::Clock::create(HSQUIRRELVM vm)
{
    auto clock_udata = make_userdata(vm, Clock());
    clock_udata.push();
    return 1;
}

SQInteger ts::script_api::Clock::getElapsedTime(HSQUIRRELVM vm)
{
    Userdata<Clock> clock_udata;
    Argument_stream argument_stream(vm, "Clock::getElapsedTime");
    argument_stream(make_reader(clock_udata));

    if (argument_stream)
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - clock_udata->start_time);

        sq_pushfloat(vm, static_cast<SQFloat>(elapsed.count() / 1000000.0));
        return 1;
    }

    else
    {
        report_argument_errors(vm, argument_stream);
        return 0;
    }    
}

SQInteger ts::script_api::Clock::restart(HSQUIRRELVM vm)
{
    Userdata<Clock> clock_udata;
    Argument_stream argument_stream(vm, "Clock::restart");
    argument_stream(make_reader(clock_udata));

    if (argument_stream)
    {
        *clock_udata = Clock();
    }

    else
    {
        report_argument_errors(vm, argument_stream);
    }

    return 0;
}