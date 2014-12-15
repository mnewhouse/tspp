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
#include "stage_api.hpp"
#include "event_api.hpp"

#include "action/stage_interface.hpp"
#include "action/stage.hpp"

#include "script/event_interface.hpp"
#include "script/argument_stream.hpp"
#include "script/script_userdata.hpp"
#include "script/script_delegates.hpp"

#include "world/world_listener.hpp"

namespace ts
{
    namespace script_api
    {
        using namespace script;

        namespace events
        {
            static const char* const onGameStart = "onGameStart";
            static const char* const onGameEnd = "onGameEnd";
            static const char* const onGameUpdate = "onGameUpdate";
        }

		namespace stage
		{
            SQInteger getStage(HSQUIRRELVM vm);

			SQInteger getGameTime(HSQUIRRELVM vm);
			SQInteger startGameTimer(HSQUIRRELVM vm);
            SQInteger getCarById(HSQUIRRELVM vm);

			static const Static_function_definition static_functions[] =
			{
				{ "Stage", getStage }
			};

			static const Member_function_definition member_functions[] =
			{
				{ "getGameTime", getGameTime },
				{ "startGameTimer", startGameTimer },
                { "getCarById", getCarById }
			};
		}

		template <>
		struct Delegate_traits<const action::Stage*>
		{
			static Range<const Member_function_definition*> member_functions();
		};
    }

	namespace script
	{
		template <>
		const char* userdata_type_name<const action::Stage*>()
		{
			return "Stage";
		}
	}
}

ts::Range<const ts::script::Member_function_definition*> ts::script_api::Delegate_traits<const ts::action::Stage*>::member_functions()
{
	return make_range(std::begin(stage::member_functions), std::end(stage::member_functions));
}

ts::script::API_definition ts::script_api::stage_api(const action::Stage_interface* stage_interface)
{
    API_definition api_def;
    api_def.interfaces.push_back(make_interface(stage_interface));
    api_def.static_functions.assign(std::begin(stage::static_functions), std::end(stage::static_functions));
    api_def.delegates.push_back(create_delegate_definition<const action::Stage*>());

    return api_def;
}

SQInteger ts::script_api::stage::getGameTime(HSQUIRRELVM vm)
{
	Userdata<const action::Stage*> stage_udata;

	Argument_stream argument_stream(vm, "Stage::getGameTime");
	argument_stream(make_reader(stage_udata));

	if (argument_stream)
	{
		auto game_time = (*stage_udata)->game_time();
		sq_pushinteger(vm, game_time);
		return 1;
	}

	else
	{
		report_argument_errors(vm, argument_stream);
		return 0;
	}

    return 0;
}


SQInteger ts::script_api::stage::startGameTimer(HSQUIRRELVM vm)
{
	Userdata<const action::Stage*> stage_udata;

	Argument_stream argument_stream(vm, "Stage::startGameTimer");
	argument_stream(make_reader(stage_udata));

	if (argument_stream)
	{
		(*stage_udata)->start_game_timer();
	}

	else
	{
		report_argument_errors(vm, argument_stream);
		return 0;
	}
    
    return 0;
}

SQInteger ts::script_api::stage::getCarById(HSQUIRRELVM vm)
{
    Userdata<const action::Stage*> stage_udata;
    std::uint16_t car_id = 0;

    Argument_stream argument_stream(vm, "Stage::getCarById");
    argument_stream(make_reader(stage_udata));
    argument_stream(make_integer_reader(car_id));

    if (argument_stream)
    {
        auto stage = *stage_udata;
        const auto* car = stage->get_car_by_id(car_id);

        if (car)
        {
            auto car_udata = make_userdata(vm, car);
            car_udata.push();
        }

        else
        {
            sq_pushnull(vm);
        }

        return 1;
    }

    else
    {
        report_argument_errors(vm, argument_stream);
        return 0;
    }
}

SQInteger ts::script_api::stage::getStage(HSQUIRRELVM vm)
{
	auto stage_interface = get_interface<const action::Stage_interface>(vm);
	const action::Stage* stage = stage_interface->stage();
	if (stage)
	{
		auto stage_udata = make_userdata(vm, stage);
		stage_udata.push();
		return 1;		
	}

	return 0;
}
