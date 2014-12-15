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
#include "common_api_errors.hpp"

#include "script/script_module.hpp"
#include "script/script_utility.hpp"

void ts::script_api::report_unavailable_function(HSQUIRRELVM vm, const utf8_string& function_name)
{
    auto module = script::get_module_by_vm(vm);

    module->report_error("The function '" + function_name + "' cannot be used at this time.", script::Error_level::runtime_error);
}