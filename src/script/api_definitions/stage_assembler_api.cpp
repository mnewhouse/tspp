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

            struct Car
            {
                std::uint32_t car_id;
            };
        }
        
        template <>
        struct Delegate_traits<server::Stage_assembler*>
        {
            static Range<const Member_function_definition*> member_functions();
        };
    }
}

ts::Range<const ts::script::Member_function_definition*> ts::script_api::Delegate_traits<ts::server::Stage_assembler*>::member_functions()
{
    return make_range(std::begin(stage_assembler::member_functions), std::end(stage_assembler::member_functions));
}

ts::script::API_definition ts::script_api::stage_assembler_api()
{
    API_definition result;
    result.delegates.push_back(create_delegate_definition<server::Stage_assembler*>());

    return result;
}

SQInteger ts::script_api::stage_assembler::addCar(HSQUIRRELVM vm)
{
    Userdata<server::Stage_assembler*> stage_assembler_udata;
    Userdata<resources::Car_handle> car_handle_udata;
    Userdata<resources::Player_color> player_color_udata;
    utf8_string_view associated_name;
    
    Argument_stream argument_stream(vm);    
    argument_stream(Userdata_reader<server::Stage_assembler*>(stage_assembler_udata));
    argument_stream(Tostring_reader(associated_name));
    argument_stream(Userdata_reader<resources::Car_handle>(car_handle_udata));
    argument_stream(Userdata_reader<resources::Player_color>(player_color_udata));

    if (argument_stream && *car_handle_udata)
    {
        resources::Player_definition player_def;
        player_def.color = *player_color_udata;
        player_def.name = associated_name;
        player_def.id = 0;

        auto stage_assembler = *stage_assembler_udata;

        stage_assembler::Car car;
        car.car_id = stage_assembler->add_car(player_def, *car_handle_udata, cup::Player_handle());
        make_userdata(vm, car).push();
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
    Userdata<stage_assembler::Car> car_udata;

    Argument_stream argument_stream(vm);    
    argument_stream(Userdata_reader<server::Stage_assembler*>(stage_assembler_udata));
    argument_stream(Userdata_reader<stage_assembler::Car>(car_udata));
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
    argument_stream(Userdata_reader<server::Stage_assembler*>(stage_assembler_udata));

    if (argument_stream)
    {
        auto stage_assembler = *stage_assembler_udata;
        const auto& stage_data = stage_assembler->stage_data();

        sq_newarray(vm, stage_data.cars.size());

        for (const auto& car : stage_data.cars)
        {
            Stack_guard loop_guard(vm);

            stage_assembler::Car entry;
            entry.car_id = car.car_id;

            make_userdata(vm, entry).push();
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