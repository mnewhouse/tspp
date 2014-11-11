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

#ifndef SCRIPT_EXECUTION_CONTEXT_HPP
#define SCRIPT_EXECUTION_CONTEXT_HPP

#include "squirrel_include.hpp"
#include "script_utility.hpp"
#include "script_value.hpp"

namespace ts
{
    namespace script
    {
        class Execution_context
        {
        public:
            Execution_context(HSQUIRRELVM vm);

            template <typename... Args>
            void operator()(const Function& function, Args&&... args) const;

        private:
            template <typename T, typename... Args>
            std::int32_t push_arguments(T&& value, Args&&... args) const;
            std::int32_t push_arguments() const;

            std::int32_t push(const Value& value) const;

            template <typename T>
            std::int32_t push(const std::vector<T>& values) const;

            HSQUIRRELVM vm_;
        };
    }
}

template <typename... Args>
void ts::script::Execution_context::operator()(const Function& function, Args&&... args) const
{
    Stack_guard stack_guard(vm_);

    function.push();
    auto arg_count = push_arguments(std::forward<Args>(args)...);

    sq_call(vm_, arg_count, SQTrue, SQTrue);
}

template <typename T, typename... Args>
std::int32_t ts::script::Execution_context::push_arguments(T&& value, Args&&... args) const
{
    auto result = push(std::forward<T>(value));
    return push_arguments(std::forward<Args>(args)...) + result;
}

template <typename T>
std::int32_t ts::script::Execution_context::push(const std::vector<T>& values) const
{
    std::int32_t result = 0;
    for (const auto& value : values)
    {
        result += push(value);
    }

    return result;
}

#endif