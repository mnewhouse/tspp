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

#ifndef SCRIPT_ENTITY_API_HPP
#define SCRIPT_ENTITY_API_HPP

#include "script/script_api.hpp"

namespace ts
{
    namespace world
    {
        class Entity;
    }

    namespace script
    {
        template <typename T>
        class Userdata;
    }

    namespace script_api
    {
        script::API_definition entity_api();

        using Entity_userdata = script::Userdata<const world::Entity*>;
        Entity_userdata make_entity_userdata(HSQUIRRELVM vm, const world::Entity* entity);
    }
}

#endif