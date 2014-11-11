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

#ifndef SCRIPT_MEMBER_FUNCTION_DEFINITION_HPP
#define SCRIPT_MEMBER_FUNCTION_DEFINITION_HPP

#include "squirrel_include.hpp"

namespace ts
{
    namespace script
    {
        struct Member_function_definition
        {
            Member_function_definition(const char* name, SQFUNCTION function, bool is_static = false)
                : name(name), function(function), is_static(is_static)
            {}

            const char* name;
            SQFUNCTION function;
            bool is_static;
        };
    }
}

#endif