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
#include "script_interface.hpp"
#include "script_utility.hpp"

namespace
{
    int interface_table_key;
}

void ts::script::register_interface(HSQUIRRELVM vm, const Generic_interface& interface)
{
    Stack_guard stack_guard(vm);

    sq_pushconsttable(vm);
    sq_pushuserpointer(vm, &interface_table_key);
    if (SQ_FAILED(sq_get(vm, -2)))
    {
        sq_newtable(vm);
        sq_pushuserpointer(vm, &interface_table_key);
        sq_push(vm, -2);
        sq_newslot(vm, -4, SQFalse);
    }

    sq_pushuserpointer(vm, interface.tag());
    sq_pushuserpointer(vm, interface.get());
    sq_newslot(vm, -3, SQFalse);
}

ts::script::Generic_interface ts::script::get_interface(HSQUIRRELVM vm, SQUserPointer type_tag)
{
    Stack_guard stack_guard(vm);

    sq_pushconsttable(vm);
    sq_pushuserpointer(vm, &interface_table_key);
    if (SQ_SUCCEEDED(sq_get(vm, -2)))
    {
        sq_pushuserpointer(vm, type_tag);
        if (SQ_SUCCEEDED(sq_get(vm, -2)))
        {
            SQUserPointer pointer;
            if (SQ_SUCCEEDED(sq_getuserpointer(vm, -1, &pointer)))
            {
                return Generic_interface(pointer, type_tag);
            }
        }
    }

    return Generic_interface();
}
