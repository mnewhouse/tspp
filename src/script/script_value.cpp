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
#include "script_value.hpp"
#include "script_function.hpp"

struct ts::script::Value::type_visitor 
    : public boost::static_visitor<ts::script::Value::Type>
{
    Type operator()(null_t) const { return null_type; }
    Type operator()(bool_t) const { return bool_type; }
    Type operator()(integer_t) const { return integer_type; }
    Type operator()(float_t) const { return float_type; }
    Type operator()(const string_t&) const { return string_type; }
    Type operator()(const string_view_t&) const { return string_view_type; }
    Type operator()(const userdata_t&) const { return userdata_type; }
    Type operator()(userpointer_t) const { return userpointer_type; }
    Type operator()(const function_t& function) const { return function_type; }
    Type operator()(const object_t& object) const { return object_type; }
};

struct ts::script::Value::push_visitor
    : public boost::static_visitor<void>
{
    push_visitor(HSQUIRRELVM vm)
      : vm(vm)
    {}

    void operator()(null_t) const { sq_pushnull(vm); }
    void operator()(bool_t value) const { sq_pushbool(vm, value); }
    void operator()(integer_t value) const { sq_pushinteger(vm, value); }
    void operator()(float_t value) const { sq_pushfloat(vm, value); }
    void operator()(const string_t& value) const { sq_pushstring(vm, value.data(), value.size()); }
    void operator()(const string_view_t& value) const { sq_pushstring(vm, value.data(), value.size()); }
    
    void operator()(userpointer_t value) const { sq_pushuserpointer(vm, value); }
    void operator()(const userdata_t& value) const { value.push(); }
    void operator()(const function_t& function) const { function.push(); }
    void operator()(const object_t& object) const { object.push(); }

    HSQUIRRELVM vm;
};

ts::script::Value::Value()
: variant_(null_t())
{
}

ts::script::Value::Value(HSQUIRRELVM vm, SQInteger index)
: variant_(read_value_from_stack(vm, index))
{
}

void ts::script::Value::push(HSQUIRRELVM vm) const
{
    boost::apply_visitor(push_visitor(vm), variant_);
}


ts::script::Value::Type ts::script::Value::type() const
{
    return boost::apply_visitor(type_visitor(), variant_);
}

ts::script::Value::variant_t ts::script::Value::read_value_from_stack(HSQUIRRELVM vm, SQInteger index)
{
    switch (sq_gettype(vm, index))    
    {
    case OT_BOOL:
    {
                    SQBool bool_value;
                    sq_getbool(vm, index, &bool_value);
                    return !!bool_value;
    }
        
    case OT_INTEGER:
    {
                       integer_t integer{};
                       sq_getinteger(vm, index, &integer);

                       return integer;
    }

    case OT_FLOAT:
    {
                     float_t float_value;
                     sq_getfloat(vm, index, &float_value);
                     return float_value;
    }

    case OT_STRING:
    {
                      const char* char_ptr{};
                      sq_getstring(vm, index, &char_ptr);
                      auto size = sq_getsize(vm, index);

                      return string_view_t(char_ptr, char_ptr + size);
    }

    case OT_USERDATA:
    {
                        return userdata_t(vm, index);
    }
        
    case OT_CLOSURE:
    case OT_NATIVECLOSURE:
    {
                             return function_t(vm, index);
    }


    default:
        return null_t();
    }
}