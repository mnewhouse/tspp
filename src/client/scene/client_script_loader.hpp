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

#ifndef CLIENT_SCRIPT_LOADER_HPP
#define CLIENT_SCRIPT_LOADER_HPP


#include "client_script_conductor.hpp"

#include "resources/generic_loader.hpp"
#include "resources/script_resource.hpp"

namespace ts
{
    namespace script
    {
        class Engine;
        class Module_loader;
    }

    namespace scene
    {
        enum class Script_loader_state
        {   
            None,
            Loading
        };

        class Script_loader
        {
        public:
            Script_loader(script::Engine* engine);
            ~Script_loader();

            const resources::Loading_interface* loading_interface() const;

            void poll();            

            void async_load_scripts(std::vector<resources::Script_handle> scripts, std::function<void()> callback);

            Script_conductor transfer_result();

        private:
            struct Impl;
            std::unique_ptr<Impl> impl_;
        };
    }
}

#endif