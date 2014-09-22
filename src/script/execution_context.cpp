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
#include "execution_context.hpp"
#include "script_value.hpp"

ts::script::Execution_context::Execution_context(HSQUIRRELVM vm)
  : vm_(vm)
{
}

std::int32_t ts::script::Execution_context::push_arguments() const
{
    return 0;
}

std::int32_t ts::script::Execution_context::push(const std::vector<Value>& values) const
{
    std::int32_t result = 0;
    for (const auto& value : values)
    {
        result += push(value);
    }

    return result;
}


std::int32_t ts::script::Execution_context::push(const Value& value) const
{
    value.push(vm_);
    return 1;
}
