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
#include "script_engine.hpp"
#include "script_module.hpp"

ts::script::Module_handle::Module_handle(Module* module, const utf8_string* module_name)
: Pointer_handle<Module>(module),
  module_name_(module_name)
{
}

const ts::utf8_string& ts::script::Module_handle::module_name() const
{
    return *module_name_;
}

ts::script::Engine::~Engine()
{
}

void ts::script::Engine::register_api(const API_definition& api_definition)
{
    script_apis_.push_back(api_definition);
}

ts::script::Module_handle ts::script::Engine::create_module(utf8_string module_name)
{
    auto insertion = module_map_.emplace(std::piecewise_construct, std::forward_as_tuple(std::move(module_name)),
                                  std::forward_as_tuple(this));

    if (!insertion.second)
    {
        return Module_handle();
    }

    auto it = insertion.first;
    const auto& name = it->first;
    auto& module = it->second;    

    for (const auto& api_def : script_apis_)
    {
        module.register_api(api_def);
    }

    return Module_handle(&module, &name);
}

void ts::script::Engine::write_console_line(const utf8_string& line, Error_level error_level)
{
    for (auto& console : consoles_)
    {
        console.write_line(line);
    }
}