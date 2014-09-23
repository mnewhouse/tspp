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

#ifndef SCRIPT_UTILITY_CLASSES_HPP
#define SCRIPT_UTILITY_CLASSES_HPP

#include "script/script_api.hpp"
#include "script/class_definition.hpp"

namespace ts
{
    namespace script_api
    {
        namespace classes
        {
            static const char Rect[] = "Rect";
            static const char Vector2[] = "Vector2";
        }

        namespace members
        {
            namespace rect
            {
                static const char left[] = "left";
                static const char top[] = "top";
                static const char width[] = "width";
                static const char height[] = "height";
            }

            namespace vector2
            {
                static const char x[] = "x";
                static const char y[] = "y";
            }
        }

        script::API_definition utility_classes();

        void push_vector2(HSQUIRRELVM vm, Vector2<double> vec);
        void push_rect(HSQUIRRELVM vm, Rect<double> rect);

        struct Rect_reader
        {
        public:
            Rect_reader(Double_rect& result);

            bool operator()(HSQUIRRELVM vm, SQInteger index) const;

        private:
            Double_rect& result_;
        };
    }
}

#endif