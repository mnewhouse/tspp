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
#include "utility_classes.hpp"

#include "script/script_utility.hpp"

namespace ts
{
    namespace script_api
    {
        using namespace script;
    }
}

ts::script::API_definition ts::script_api::utility_classes()
{
    API_definition api_def;
    
    Class_definition rect;
    rect.class_name = classes::Rect;
    rect.add_member(members::rect::left);
    rect.add_member(members::rect::top);
    rect.add_member(members::rect::width);
    rect.add_member(members::rect::height);

    api_def.classes.push_back(rect);

    Class_definition vector2;
    vector2.class_name = classes::Vector2;
    vector2.add_member(members::vector2::x);
    vector2.add_member(members::vector2::y);
    api_def.classes.push_back(vector2);

    return api_def;
}

void ts::script_api::push_vector2(HSQUIRRELVM vm, Vector2<double> vec)
{
    auto class_handle = get_class_by_name(vm, classes::Vector2);
    if (class_handle)
    {
        Object_handle instance;

        {
            Stack_guard stack_guard(vm);
            class_handle.push();
            sq_createinstance(vm, -1);
            instance = Object_handle(vm, -1);

            sq_pushstring(vm, members::vector2::x, -1);
            sq_pushfloat(vm, static_cast<SQFloat>(vec.x));
            sq_set(vm, -3);
            sq_pushstring(vm, members::vector2::y, -1);
            sq_pushfloat(vm, static_cast<SQFloat>(vec.y));
            sq_set(vm, -3);
        }

        instance.push();
    }

    else
    {
        sq_pushnull(vm);
    }
}


void ts::script_api::push_rect(HSQUIRRELVM vm, Rect<double> rect)
{
    auto class_handle = get_class_by_name(vm, classes::Rect);
    if (class_handle)
    {
        Object_handle instance;

        {
            Stack_guard stack_guard(vm);
            class_handle.push();
            sq_createinstance(vm, -1);
            instance = Object_handle(vm, -1);

            sq_pushstring(vm, members::rect::left, -1);
            sq_pushfloat(vm, static_cast<SQFloat>(rect.left));
            sq_set(vm, -3);

            sq_pushstring(vm, members::rect::top, -1);
            sq_pushfloat(vm, static_cast<SQFloat>(rect.top));
            sq_set(vm, -3);
            
            sq_pushstring(vm, members::rect::width, -1);
            sq_pushfloat(vm, static_cast<SQFloat>(rect.width));
            sq_set(vm, -3);

            sq_pushstring(vm, members::rect::height, -1);
            sq_pushfloat(vm, static_cast<SQFloat>(rect.height));
            sq_set(vm, -3);
        }

        instance.push();
    }

    else
    {
        sq_pushnull(vm);
    }
}

ts::script_api::Rect_reader::Rect_reader(Double_rect& result)
: result_(result)
{
}

bool ts::script_api::Rect_reader::operator()(HSQUIRRELVM vm, SQInteger index) const
{
    Object_handle rect_object;
    Instance_reader instance_reader(classes::Rect, rect_object);
    if (instance_reader(vm, index))
    {
        Rect<SQFloat> rect;

        sq_pushstring(vm, members::rect::left, -1);
        sq_get(vm, index);
        if (SQ_FAILED(sq_getfloat(vm, -1, &rect.left))) return false;        

        sq_pushstring(vm, members::rect::top, -1);
        sq_get(vm, index);
        if (SQ_FAILED(sq_getfloat(vm, -1, &rect.top))) return false;

        sq_pushstring(vm, members::rect::width, -1);
        sq_get(vm, index);
        if (SQ_FAILED(sq_getfloat(vm, -1, &rect.width))) return false;

        sq_pushstring(vm, members::rect::height, -1);
        sq_get(vm, index);
        if (SQ_FAILED(sq_getfloat(vm, -1, &rect.height))) return false;

        result_.left = static_cast<double>(rect.left);
        result_.top = static_cast<double>(rect.top);
        result_.width = static_cast<double>(rect.width);
        result_.height = static_cast<double>(rect.height);
        return true;
    }

    return false;
}