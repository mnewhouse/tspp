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

#ifndef MODULE_LOADER_HPP
#define MODULE_LOADER_HPP

#include "module_handle.hpp"

namespace ts
{
    namespace script
    {
        class Engine;

        struct Module_definition
        {
            utf8_string module_name;
            std::vector<utf8_string> file_list;
        };

        class Module_loader
        {
        public:
            explicit Module_loader(Engine* engine);
            ~Module_loader();

            using Completion_callback = std::function<void(std::vector<Unique_module_handle>)>;

            void async_load_modules(std::vector<Module_definition> modules, Completion_callback callback);

            void poll();

        private:
            struct Loading_state
            {
                std::future<void> future;
                std::vector<Unique_module_handle> modules;
                std::vector<Module_definition> definitions;
                Completion_callback completion_callback;
            };

            std::vector<Unique_module_handle> execute_modules(std::vector<Unique_module_handle> modules);

            std::list<Loading_state> loading_states_;
            script::Engine* engine_;
        };
    }
}

#endif