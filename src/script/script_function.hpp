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

#ifndef SCRIPT_FUNCTION_HPP
#define SCRIPT_FUNCTION_HPP

#include "squirrel_include.hpp"
#include "script_utility.hpp"

namespace ts
{
    namespace script
    {
        class Value;

        class Function
        {
        public:
            Function();
            Function(HSQUIRRELVM vm, SQInteger stack_idx);

            Function(const Function& other);
            Function(Function&& other);

            ~Function();

            Function& operator=(Function rhs);

            template <typename... Args>
            void operator()(Args&&... args) const;
            
            explicit operator bool() const;

            void push() const;

        private:
            HSQUIRRELVM vm_;
            HSQOBJECT object_;
        };
    }
}

template <typename... Args>
void ts::script::Function::operator()(Args&&... args) const
{
    get_module_by_vm(vm_)->call_function(*this, std::forward<Args>(args)...);
}

#endif