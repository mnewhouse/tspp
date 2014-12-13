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

#ifndef SCRIPT_ENGINE_HPP
#define SCRIPT_ENGINE_HPP

#include "squirrel_include.hpp"
#include "script_api.hpp"
#include "script_console.hpp"
#include "module_handle.hpp"

namespace ts
{
    namespace script
    {
        class Engine
        {
        public:
            Engine();
            ~Engine();

            Engine(const Engine&) = delete;
            Engine& operator=(const Engine&) = delete;

            Unique_module_handle create_module(utf8_string module_name, take_ownership_t);
            Module_handle create_module(utf8_string module_name);

            void unload_module(Module_handle module_handle);
            void unload_all_modules();

            Module_handle get_module_by_name(const utf8_string& module_name);

            void register_api(const API_definition& api_definition);
            const std::vector<API_definition>& api_definitions() const;

            template <typename ConsoleType>
            void register_console(ConsoleType console);

            void write_console_line(const utf8_string& string, Error_level error_level = Error_level::none);   

            template <typename... Args>
            void trigger_event(const utf8_string& event_name, Args&&... args) const;

        private:
            std::vector<Console> consoles_;
            std::vector<API_definition> script_apis_;

            std::map<utf8_string, Module> module_map_;
        };
    }
}

template <typename ConsoleType>
void ts::script::Engine::register_console(ConsoleType console)
{
    consoles_.emplace_back(std::move(console));
}

template <typename... Args>
void ts::script::Engine::trigger_event(const utf8_string& event_name, Args&&... args) const
{
    for (auto& module : module_map_)
    {
        module.second.trigger_event(event_name, std::forward<Args>(args)...);
    }
}

#endif