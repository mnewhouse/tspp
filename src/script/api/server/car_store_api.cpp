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
#include "car_store_api.hpp"

#include "resources/car_store.hpp"
#include "resources/car_model.hpp"

#include "script/script_delegates.hpp"
#include "script/script_userdata.hpp"
#include "script/argument_stream.hpp"

namespace ts
{
    namespace script_api
    {
        using namespace script;

        struct Car_store
        {
            const resources::Car_store* local_car_store;
            const resources::Car_store* global_car_store;
        };

        namespace car_store
        {
            SQInteger getCarByName(HSQUIRRELVM vm);
            SQInteger getLocalCars(HSQUIRRELVM vm);
            SQInteger getCars(HSQUIRRELVM vm);

            static const Member_function_definition member_functions[] =
            {
                { "getCarByName", getCarByName },
                { "getLocalCars", getLocalCars },
                { "getCars", getCars }
            };

            SQInteger CarStore(HSQUIRRELVM vm);
            static const Static_function_definition static_functions[] = 
            {
                { "CarStore", CarStore }
            };
        }

        namespace car_model
        {
            SQInteger getCarName(HSQUIRRELVM vm);

            static const Member_function_definition member_functions[] =
            {
                { "getCarName", getCarName },
            };
        }

        template <>
        struct Delegate_traits<Car_store>
        {
            static Range<const Member_function_definition*> member_functions();
        };

        template <>
        struct Delegate_traits<resources::Car_model>
        {
            static Range<const Member_function_definition*> member_functions();
        };
    }
}

ts::Range<const ts::script::Member_function_definition*> ts::script_api::Delegate_traits<ts::script_api::Car_store>::member_functions()
{
    return make_range(std::begin(car_store::member_functions), std::end(car_store::member_functions));
}

ts::Range<const ts::script::Member_function_definition*> ts::script_api::Delegate_traits<ts::resources::Car_model>::member_functions()
{
    return make_range(std::begin(car_model::member_functions), std::end(car_model::member_functions));
}

ts::script::API_definition ts::script_api::car_store_api(const resources::Car_store* global_car_store)
{
    API_definition result;
    result.interfaces.push_back(make_interface(global_car_store));
    result.delegates.push_back(create_delegate_definition<Car_store>());
    result.delegates.push_back(create_delegate_definition<resources::Car_model>());
    result.static_functions.assign(std::begin(car_store::static_functions), std::end(car_store::static_functions));
    return result;
}

SQInteger ts::script_api::car_store::CarStore(HSQUIRRELVM vm)
{    
    Car_store car_store;
    car_store.local_car_store = nullptr;
    car_store.global_car_store = get_interface<const resources::Car_store>(vm).get();

    auto resource = get_interface<const resources::Script_resource>(vm);
    if (resource)
    {
        car_store.local_car_store = resource->car_store();
    }

    make_userdata(vm, car_store).push();
    return 1;
}

SQInteger ts::script_api::car_store::getCarByName(HSQUIRRELVM vm)
{
    Userdata<Car_store> car_store_udata;
    utf8_string_view car_name;

    Argument_stream argument_stream(vm, "CarStore::getCarByName");
    argument_stream(Userdata_reader<Car_store>(car_store_udata));
    argument_stream(Tostring_reader(car_name));

    if (argument_stream)
    {
        auto local_car_store = car_store_udata->local_car_store;
        auto global_car_store = car_store_udata->global_car_store;

        if (local_car_store)
        {
            if (auto car_handle = local_car_store->get_car_by_name(car_name))
            {
                resources::Car_model car_model;
                car_model.model = car_handle;
                car_model.resource = get_interface<const resources::Script_resource>(vm);
                make_userdata(vm, car_model).push();
                return 1;
            }
        }



        if (auto car_handle = global_car_store->get_car_by_name(car_name))
        {
            resources::Car_model model;
            model.model = car_handle;
            make_userdata(vm, model).push();
            return 1;
        }
    }

    else
    {
        report_argument_errors(vm, argument_stream);
    }

    sq_pushnull(vm);
    return 1;
}

SQInteger ts::script_api::car_store::getLocalCars(HSQUIRRELVM vm)
{
    Userdata<Car_store> car_store_udata;
    
    Argument_stream argument_stream(vm, "CarStore::getLocalCars");
    argument_stream(Userdata_reader<Car_store>(car_store_udata));

    if (argument_stream)
    {
        sq_newarray(vm, 0);

        for (auto car_handle : *car_store_udata->local_car_store)
        {
            Stack_guard loop_guard(vm);
            make_userdata(vm, car_handle).push();
            sq_arrayappend(vm, 0);
        }

        return 1;
    }

    else
    {
        report_argument_errors(vm, argument_stream);
        return 0;
    }
}


SQInteger ts::script_api::car_store::getCars(HSQUIRRELVM vm)
{
    Userdata<Car_store> car_store_udata;
    
    Argument_stream argument_stream(vm, "CarStore::getCars");
    argument_stream(Userdata_reader<Car_store>(car_store_udata));

    if (argument_stream)
    {
        sq_newarray(vm, 0);

        for (auto car_handle : *car_store_udata->local_car_store)
        {
            Stack_guard loop_guard(vm);
            make_userdata(vm, car_handle).push();
            sq_arrayappend(vm, -2);
        }

        for (auto car_handle : *car_store_udata->global_car_store)
        {
            Stack_guard loop_guard(vm);
            make_userdata(vm, car_handle).push();
            sq_arrayappend(vm, -2);
        }

        return 1;
    }

    else
    {
        report_argument_errors(vm, argument_stream);
        return 0;
    }
}

SQInteger ts::script_api::car_model::getCarName(HSQUIRRELVM vm)
{
    Userdata<resources::Car_model> car_model_udata;
    
    Argument_stream argument_stream(vm, "CarModel::getCarName");
    argument_stream(make_reader(car_model_udata));

    if (argument_stream && car_model_udata->model)
    {
        const auto& car_name = car_model_udata->model->car_name;
        sq_pushstring(vm, car_name.data(), car_name.size());
        return 1;
    }

    else
    {
        report_argument_errors(vm, argument_stream);
        return 0;
    }    
}