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
#include "object_handle.hpp"

ts::script::Object_handle::Object_handle()
: vm_(nullptr)
{
    object_._type = OT_NULL;
}

ts::script::Object_handle::Object_handle(HSQUIRRELVM vm, SQInteger index, SQObjectType needed_type)
: vm_(nullptr)
{
    reset(vm, index, needed_type);
}

ts::script::Object_handle::Object_handle(const Object_handle& other)
: vm_(other.vm_),
  object_(other.object_)
{
    if (other)
    {
        sq_addref(vm_, &object_);
    }
}

ts::script::Object_handle::Object_handle(Object_handle&& other)
: vm_(other.vm_),
  object_(other.object_)
{
    other = Object_handle();
}

ts::script::Object_handle::~Object_handle()
{
    if (*this)
    {
        sq_release(vm_, &object_);
    }
}

ts::script::Object_handle& ts::script::Object_handle::operator=(Object_handle other)
{
    std::swap(vm_, other.vm_);
    std::swap(object_, other.object_);

    return *this;
}


void ts::script::Object_handle::push() const
{
    sq_pushobject(vm_, object_);
}

SQObjectType ts::script::Object_handle::type() const
{
    return object_._type;
}

void ts::script::Object_handle::reset()
{
    if (*this)
    {
        sq_release(vm_, &object_);
    }

    vm_ = nullptr;
    object_._type = OT_NULL;
}

void ts::script::Object_handle::reset(HSQUIRRELVM vm, SQInteger index, SQObjectType needed_type)
{
    if (*this)
    {
        sq_release(vm_, &object_);
    }

    vm_ = vm;
    HSQOBJECT object;

    if (SQ_SUCCEEDED(sq_getstackobj(vm, index, &object)))
    {
        if (needed_type == OT_NULL || object._type == needed_type)
        {
            object_ = object;
            sq_addref(vm, &object_);
        }        
    }
}

HSQUIRRELVM ts::script::Object_handle::vm() const
{
    return vm_;
}

ts::script::Object_handle::operator bool() const
{
    return vm_ != nullptr && object_._type != OT_NULL;
}