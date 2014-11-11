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

#ifndef SCRIPT_CLASS_DEFINITION_HPP
#define SCRIPT_CLASS_DEFINITION_HPP

#include "squirrel_include.hpp"
#include "member_function.hpp"
#include "object_handle.hpp"

namespace ts
{
    namespace script
    {
        struct Class_member
        {
            Class_member(const char* name, bool is_static = false)
                : name(name), is_static(is_static)
            {}

            const char* name;
            bool is_static;
        };

        struct Class_definition
        {
            const char* class_name;
            const char* base_class_name = nullptr;

            void add_member(const char* name, bool is_static = false)
            {
                members.emplace_back(name, is_static);
            }

            void add_member_function(const char* name, SQFUNCTION function, bool is_static = false)
            {
                member_functions.emplace_back(name, function, is_static);
            }

            std::vector<Class_member> members;
            std::vector<Member_function_definition> member_functions;
        };

        void register_class_definition(HSQUIRRELVM vm, const Class_definition& class_def);
        Object_handle get_class_by_name(HSQUIRRELVM vm, const char* class_name);

        struct Instance_reader
        {
            Instance_reader(const char* builtin_class_name, Object_handle& object_handle);

            bool operator()(HSQUIRRELVM vm, SQInteger index) const;

        private:
            const char* builtin_class_name_;
            Object_handle& object_handle_;            
        };
    }
}

#endif