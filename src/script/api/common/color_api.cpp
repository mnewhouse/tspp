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
#include "color_api.hpp"

#include "script/argument_stream.hpp"

namespace ts
{
    namespace script_api
    {
        using namespace script;

        namespace members
        {
            namespace color
            {
                SQInteger constructor(HSQUIRRELVM vm);
            }
        }
    }
}

ts::script::API_definition ts::script_api::color_api()
{
    API_definition result;

    Class_definition color;
    color.class_name = classes::Color;
    color.add_member(members::color::r);
    color.add_member(members::color::g);
    color.add_member(members::color::b);
    color.add_member(members::color::a);

    color.add_member_function("constructor", members::color::constructor, false);
    result.classes.push_back(color);

    return result;
}

SQInteger ts::script_api::members::color::constructor(HSQUIRRELVM vm)
{
    Object_handle color_instance;
    double red = 0.0, green = 0.0, blue = 0.0, alpha = 255.0;

    Stack_guard stack_guard(vm);

    Argument_stream argument_stream(vm, "Color::constructor");
    argument_stream(Instance_reader(classes::Color, color_instance));
    argument_stream(Numeric_reader(red), arg::optional);
    argument_stream(Numeric_reader(green), arg::optional);
    argument_stream(Numeric_reader(blue), arg::optional);
    argument_stream(Numeric_reader(alpha), arg::optional);

    if (argument_stream)
    {
        color_instance.push();
        red = clamp(red, 0.0, 255.0);
        green = clamp(green, 0.0, 255.0);
        blue = clamp(blue, 0.0, 255.0);
        alpha = clamp(alpha, 0.0, 255.0);

        sq_pushstring(vm, members::color::r, -1);
        sq_pushfloat(vm, static_cast<SQFloat>(red));
        sq_set(vm, -3);

        sq_pushstring(vm, members::color::g, -1);
        sq_pushfloat(vm, static_cast<SQFloat>(green));
        sq_set(vm, -3);

        sq_pushstring(vm, members::color::b, -1);
        sq_pushfloat(vm, static_cast<SQFloat>(blue));
        sq_set(vm, -3);

        sq_pushstring(vm, members::color::a, -1);
        sq_pushfloat(vm, static_cast<SQFloat>(alpha));
        sq_set(vm, -3);
    }

    else
    {
        report_argument_errors(vm, argument_stream);
    }

    return 0;
}

ts::script_api::Color_reader::Color_reader(sf::Color& result)
: result_(result)
{
}

bool ts::script_api::Color_reader::operator()(HSQUIRRELVM vm, SQInteger index) const
{
    Object_handle color_instance;
    Stack_guard stack_guard(vm);

    if (Instance_reader(classes::Color, color_instance)(vm, index))
    {
        double red = 0.0, green = 0.0, blue = 0.0, alpha = 0.0;
        color_instance.push();
        sq_pushstring(vm, members::color::r, -1);

        if (SQ_FAILED(sq_get(vm, -2)) || !Numeric_reader(red)(vm, -1)) return false;

        color_instance.push();
        sq_pushstring(vm, members::color::g, -1);
        if (SQ_FAILED(sq_get(vm, -2)) || !Numeric_reader(green)(vm, -1)) return false;

        color_instance.push();
        sq_pushstring(vm, members::color::b, -1);
        if (SQ_FAILED(sq_get(vm, -2)) || !Numeric_reader(blue)(vm, -1)) return false;

        color_instance.push();
        sq_pushstring(vm, members::color::a, -1);
        if (SQ_FAILED(sq_get(vm, -2)) || !Numeric_reader(alpha)(vm, -1)) return false;

        
        result_.r = static_cast<sf::Uint8>(clamp(red, 0.0, 255.0));
        result_.g = static_cast<sf::Uint8>(clamp(green, 0.0, 255.0));
        result_.b = static_cast<sf::Uint8>(clamp(blue, 0.0, 255.0));
        result_.a = static_cast<sf::Uint8>(clamp(alpha, 0.0, 255.0));

        return true;
    }

    return false;
}