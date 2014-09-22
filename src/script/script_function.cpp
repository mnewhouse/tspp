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
#include "script_function.hpp"

ts::script::Function::Function()
: vm_(nullptr)
{
    object_._type = OT_NULL;
}

ts::script::Function::Function(HSQUIRRELVM vm, SQInteger stack_index)
: vm_(vm),
  object_()
{
    object_._type = OT_NULL;

    auto object_type = sq_gettype(vm, stack_index);
    if (object_type == OT_CLOSURE || object_type == OT_NATIVECLOSURE)
    {
        if (SQ_SUCCEEDED(sq_getstackobj(vm, stack_index, &object_)))
        {
            sq_addref(vm, &object_);
        }
    }
}

ts::script::Function::Function(Function&& other)
: vm_(other.vm_),
  object_(other.object_)
{
    other.vm_ = nullptr;
    other.object_._type = OT_NULL;
}

ts::script::Function::Function(const Function& other)
: vm_(other.vm_),
  object_(other.object_)
{
    sq_addref(vm_, &object_);
}

ts::script::Function::~Function()
{
    sq_release(vm_, &object_);
}

ts::script::Function& ts::script::Function::operator=(Function rhs)
{
    std::swap(vm_, rhs.vm_);
    std::swap(object_, rhs.object_);

    return *this;
}

ts::script::Function::operator bool() const
{
    return object_._type == OT_CLOSURE || object_._type == OT_NATIVECLOSURE;
}


void ts::script::Function::push() const
{
    sq_pushobject(vm_, object_);
}

