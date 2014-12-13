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

#ifndef SCRIPT_API_HPP
#define SCRIPT_API_HPP

#include "squirrel_include.hpp"
#include "class_definition.hpp"
#include "delegate_definition.hpp"
#include "script_interface.hpp"

namespace ts
{
    namespace script
    {
        struct Static_function_definition
        {
            const char* name;
            SQFUNCTION function;
        };

        using Launch_callback = std::function<void(HSQUIRRELVM)>;

        struct API_definition
        {
            std::vector<Static_function_definition> static_functions;
            std::vector<Delegate_definition> delegates;
            std::vector<Class_definition> classes;
            std::vector<Launch_callback> launch_callbacks;
            std::vector<Generic_interface> interfaces;
        };
    };
}

#endif