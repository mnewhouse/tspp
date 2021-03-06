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

#pragma once

#ifndef SCRIPT_DELEGATE_DEFINITION_HPP
#define SCRIPT_DELEGATE_DEFINITION_HPP

#include "squirrel_include.hpp"
#include "member_function.hpp"
#include "script_table.hpp"

namespace ts
{
    namespace script
    {
        struct Delegate_definition
        {
            std::vector<Member_function_definition> member_functions;
            SQUserPointer type_tag;
        };

        void create_delegate_table(HSQUIRRELVM vm, const Delegate_definition& definition);
        Table get_delegate_table(HSQUIRRELVM vm, SQUserPointer type_tag);

        template <typename T>
        Table get_delegate_table(HSQUIRRELVM vm)
        {
            return get_delegate_table(vm, userdata_type_tag<T>());
        }
    }
}

#endif
