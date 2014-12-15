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
#include "entity_api.hpp"
#include "utility_classes.hpp"

#include "script/script_delegates.hpp"
#include "script/script_userdata.hpp"
#include "script/argument_stream.hpp"

#include "world/entity.hpp"
#include "world/car.hpp"

namespace ts
{
    namespace script
    {
        template <>
        struct Userdata_traits<const world::Car*>
        {

        };
    }

    namespace script_api
    {
        using namespace script;

        namespace entity
        {
            SQInteger getPosition(HSQUIRRELVM vm);
            SQInteger getVelocity(HSQUIRRELVM vm);
            SQInteger getRotation(HSQUIRRELVM vm);
            SQInteger getZPosition(HSQUIRRELVM vm);

            static const Member_function_definition member_functions[] =
            {
                { "getPosition", getPosition }
            };
        }

        template <>
        struct Delegate_traits<const world::Entity*>
        {
            static Range<const Member_function_definition*> member_functions()
            {
                return make_range(std::begin(entity::member_functions), std::end(entity::member_functions));
            }
        };
    }
}

ts::script::Userdata<const ts::world::Entity*> ts::script_api::make_entity_userdata(HSQUIRRELVM vm, const world::Entity* entity)
{
    return make_userdata(vm, entity);
}

ts::script::API_definition ts::script_api::entity_api()
{
    API_definition result;
    result.delegates.push_back(create_delegate_definition<const world::Entity*>());

    return result;    
}

SQInteger ts::script_api::entity::getPosition(HSQUIRRELVM vm)
{
    Entity_userdata entity_udata;

    Argument_stream argument_stream(vm, "Entity::getPosition");
    argument_stream(make_reader(entity_udata));

    if (argument_stream)
    {
        auto entity = *entity_udata;
        push_vector2(vm, entity->position());
        return 1;
    }

    return 0;
}