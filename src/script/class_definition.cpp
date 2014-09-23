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
#include "class_definition.hpp"
#include "script_utility.hpp"

namespace
{
    int class_table_key;
}

void ts::script::register_class_definition(HSQUIRRELVM vm, const Class_definition& class_def)
{
    Stack_guard stack_guard(vm);

    sq_pushconsttable(vm);
    sq_pushuserpointer(vm, &class_table_key);
    if (SQ_FAILED(sq_get(vm, -2)))
    {
        sq_newtable(vm);
        sq_pushuserpointer(vm, &class_table_key);
        sq_push(vm, -2);
        sq_newslot(vm, -4, SQFalse);
    }

    sq_newclass(vm, false);
    sq_pushstring(vm, class_def.class_name, -1);
    sq_push(vm, -2);
    sq_newslot(vm, -4, SQFalse);

    sq_pushstring(vm, class_def.class_name, -1);
    sq_push(vm, -3);
    sq_newslot(vm, -5, SQFalse); // const table

    for (const auto& member : class_def.members)
    {
        sq_pushstring(vm, member.name, -1);
        sq_pushnull(vm);
        sq_newslot(vm, -3, member.is_static);
    }
}

ts::script::Object_handle ts::script::get_class_by_name(HSQUIRRELVM vm, const char* class_name)
{
    Stack_guard stack_guard(vm);
    sq_pushconsttable(vm);
    sq_pushuserpointer(vm, &class_table_key);
    if (SQ_SUCCEEDED(sq_get(vm, -2)))
    {
        sq_pushstring(vm, class_name, -1);
        if (SQ_SUCCEEDED(sq_get(vm, -2)))
        {
            return Object_handle(vm, -1);
        }
    }

    return Object_handle();
}

ts::script::Instance_reader::Instance_reader(const char* builtin_class_name, Object_handle& object_handle)
: builtin_class_name_(builtin_class_name),
  object_handle_(object_handle)
{
}

bool ts::script::Instance_reader::operator()(HSQUIRRELVM vm, SQInteger index) const
{
    Stack_guard stack_guard(vm);

    Object_handle instance(vm, index, OT_INSTANCE);
    if (instance)
    {
        if (SQ_SUCCEEDED(sq_getclass(vm, index)))
        {
            Object_handle instance_class(vm, -1);
            if (instance_class == get_class_by_name(vm, builtin_class_name_))
            {
                object_handle_ = instance;
                return true;
            }
        }
    }

    return false;
}