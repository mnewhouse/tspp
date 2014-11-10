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
#include "world_api.hpp"

#include "action/stage.hpp"

#include "script/argument_stream.hpp"
#include "script/script_userdata.hpp"

namespace ts
{
    namespace script_api
    {
        using namespace script;

        SQInteger getGameTime(HSQUIRRELVM vm);
        SQInteger startGameTimer(HSQUIRRELVM vm);

        static const Static_function_definition world_functions[] =
        {
            { "getGameTime", getGameTime },
            { "startGameTimer", startGameTimer }
        };
    }
}

ts::script::API_definition ts::script_api::stage_api(const action::Stage* stage)
{
    API_definition api_def;
    api_def.interfaces.push_back(make_interface(stage));
    api_def.static_functions.assign(std::begin(world_functions), std::end(world_functions));

    return api_def;
}

SQInteger ts::script_api::getGameTime(HSQUIRRELVM vm)
{
    auto stage = get_interface<action::Stage>(vm);
    sq_pushinteger(vm, stage->game_time());
    return 1;
}


SQInteger ts::script_api::startGameTimer(HSQUIRRELVM vm)
{
    auto stage = get_interface<action::Stage>(vm);
    stage->start_game_timer();
    return 1;
}