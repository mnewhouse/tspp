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
#include "stage_assembler_api.hpp"
#include "stage_api.hpp"

#include "resources/car_handle.hpp"
#include "resources/player_definition.hpp"
#include "server/stage_assembler.hpp"
#include "cup/stage_data.hpp"

#include "script/script_delegates.hpp"
#include "script/argument_stream.hpp"

namespace ts
{
    namespace script_api
    {
        using namespace script;

        namespace stage_assembler
        {
            SQInteger addCar(HSQUIRRELVM vm);
            SQInteger removeCar(HSQUIRRELVM vm);
            SQInteger carList(HSQUIRRELVM vm);

            static const Member_function_definition member_functions[] =
            {
                { "addCar", addCar },
                { "removeCar", removeCar },
                { "carList", carList }
            };

			struct Car_info
			{
                Car_info(const server::Stage_assembler* stage_assembler, std::uint32_t car_id)
                    : car_id(car_id), stage_assembler(stage_assembler)
                {
                }

				std::uint32_t car_id;
                const server::Stage_assembler* stage_assembler;

                static SQInteger getId(HSQUIRRELVM vm);
			};

            static const Member_function_definition car_member_functions[] =
            {
                { "getId", Car_info::getId }
            };
        }
        
        template <>
        struct Delegate_traits<server::Stage_assembler*>
        {
            static Range<const Member_function_definition*> member_functions();
        };

        template <>
        struct Delegate_traits<stage_assembler::Car_info>
        {
            static Range<const Member_function_definition*> member_functions();
        };
    }
}

ts::Range<const ts::script::Member_function_definition*> ts::script_api::Delegate_traits<ts::server::Stage_assembler*>::member_functions()
{
    return make_range(std::begin(stage_assembler::member_functions), std::end(stage_assembler::member_functions));
}

ts::Range<const ts::script::Member_function_definition*> ts::script_api::Delegate_traits<ts::script_api::stage_assembler::Car_info>::member_functions()
{
    return make_range(std::begin(stage_assembler::car_member_functions), std::end(stage_assembler::car_member_functions));
}

ts::script::API_definition ts::script_api::stage_assembler_api()
{
    API_definition result;
    result.delegates.push_back(create_delegate_definition<server::Stage_assembler*>());
    result.delegates.push_back(create_delegate_definition<stage_assembler::Car_info>());

    return result;
}

SQInteger ts::script_api::stage_assembler::addCar(HSQUIRRELVM vm)
{
    Userdata<server::Stage_assembler*> stage_assembler_udata;
    Userdata<resources::Car_handle> car_handle_udata;
    Userdata<resources::Player_color> player_color_udata;
    utf8_string_view associated_name;
    
    Argument_stream argument_stream(vm);    
    argument_stream(make_reader(stage_assembler_udata));
    argument_stream(Tostring_reader(associated_name));
    argument_stream(make_reader(car_handle_udata));
    argument_stream(make_reader(player_color_udata));

    if (argument_stream && *car_handle_udata)
    {
        resources::Player_definition player_def;
        player_def.color = *player_color_udata;
        player_def.name = associated_name;
        player_def.id = 0;

        auto stage_assembler = *stage_assembler_udata;

        auto car_id = stage_assembler->add_car(player_def, *car_handle_udata, cup::Player_handle());
        stage_assembler::Car_info car_info(stage_assembler, car_id);

		auto car_info_udata = make_userdata(vm, car_info);
		car_info_udata.push();
        return 1;
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
        return 0;
    }    
}

SQInteger ts::script_api::stage_assembler::removeCar(HSQUIRRELVM vm)
{
    Userdata<server::Stage_assembler*> stage_assembler_udata;
    Userdata<stage_assembler::Car_info> car_udata;

    Argument_stream argument_stream(vm);    
    argument_stream(make_reader(stage_assembler_udata));
    argument_stream(make_reader(car_udata));
    if (argument_stream)
    {
        auto stage_assembler = *stage_assembler_udata;
        stage_assembler->remove_car_by_id(car_udata->car_id);
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
    }

    return 0;
}

SQInteger ts::script_api::stage_assembler::carList(HSQUIRRELVM vm)
{
    Userdata<server::Stage_assembler*> stage_assembler_udata;
    
    Argument_stream argument_stream(vm);    
    argument_stream(make_reader(stage_assembler_udata));

    if (argument_stream)
    {
        auto stage_assembler = *stage_assembler_udata;
        const auto& stage_data = stage_assembler->stage_data();

        sq_newarray(vm, stage_data.cars.size());

        for (const auto& car : stage_data.cars)
        {
            Stack_guard loop_guard(vm);

            stage_assembler::Car_info car_info(stage_assembler, car.car_id);

            auto car_info_udata = make_userdata(vm, car_info);
			car_info_udata.push();

            sq_arrayappend(vm, -2);
        }

        return 1;
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
        return 0;
    }    
}

SQInteger ts::script_api::stage_assembler::Car_info::getId(HSQUIRRELVM vm)
{
    Userdata<stage_assembler::Car_info> car_info_udata;

    Argument_stream argument_stream(vm);
    argument_stream(make_reader(car_info_udata));

    if (argument_stream)
    {
        sq_pushinteger(vm, car_info_udata->car_id);
        return 1;
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
        return 0;
    }
}