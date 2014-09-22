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

#ifndef SCRIPT_UTILITY_HPP
#define SCRIPT_UTILITY_HPP

#include "squirrel_include.hpp"
#include "script_table.hpp"

namespace ts
{
    namespace script
    {
        class Module;
        class Engine;

        Table get_root_table(HSQUIRRELVM);

        utf8_string get_value_string(HSQUIRRELVM vm, SQInteger index);

        struct Stack_guard
        {
        public:
            Stack_guard(HSQUIRRELVM vm);
            ~Stack_guard();

            Stack_guard(const Stack_guard&) = delete;
            Stack_guard& operator=(const Stack_guard&) = delete;

        private:
            HSQUIRRELVM vm_;
            SQInteger top_;            
        };

        
    }
}

#endif