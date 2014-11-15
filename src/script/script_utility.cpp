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
#include "script_utility.hpp"
#include "script_module.hpp"

ts::script::Stack_guard::Stack_guard(HSQUIRRELVM vm)
: vm_(vm),
  top_(sq_gettop(vm))
{
}

ts::script::Stack_guard::~Stack_guard()
{
    sq_settop(vm_, top_);
}

ts::script::Table ts::script::get_root_table(HSQUIRRELVM vm)
{
    Stack_guard stack_guard(vm);
    sq_pushroottable(vm);
    return Table(vm, -1);
}


ts::script::Module* ts::script::get_module_by_vm(HSQUIRRELVM vm)
{
    return reinterpret_cast<Module*>(sq_getforeignptr(vm));
}

ts::script::Engine* ts::script::get_engine_by_vm(HSQUIRRELVM vm)
{
    return get_module_by_vm(vm)->engine();
}

ts::utf8_string ts::script::get_value_string(HSQUIRRELVM vm, SQInteger index)
{
    auto type = sq_gettype(vm, index);
    switch (type)
    {
    case OT_NULL:
        return "null";

    case OT_BOOL:
    {
                    SQBool value;
                    sq_getbool(vm, index, &value);
                    return "bool" + std::string(value ? "(true)" : "(false)");
    }

    case OT_INTEGER:
    {
                       SQInteger value;
                       sq_getinteger(vm, index, &value);
                       return "integer(" + std::to_string(value) + ")";
    }
        

    case OT_FLOAT:
    {
                     SQFloat value;
                     sq_getfloat(vm, index, &value);
                     return "float(" + std::to_string(value) + ")";
    }
        

    case OT_STRING:
    {
                      auto size = sq_getsize(vm, index);
                      return "string:" + std::to_string(size);
    }

    case OT_USERDATA:
        return "userdata";

    case OT_USERPOINTER:
        return "userpointer";

    case OT_ARRAY:
        return "array";

    case OT_TABLE:
        return "table";

    case OT_CLASS:
        return "class";

    case OT_INSTANCE:
        return "instance";

    case OT_CLOSURE: case OT_NATIVECLOSURE:
        return "function";

    case OT_GENERATOR:
        return "generator";

    case OT_THREAD:
        return "thread";

    case OT_FUNCPROTO:
        return "prototype";

    case OT_WEAKREF:
        return "weak ref";

    case OT_OUTER:
        return "outer";

    default:
        return "";
    }
}