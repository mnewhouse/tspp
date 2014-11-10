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

#include "generic_loader.hpp"

#include "resources/script_resource.hpp"

#include "script/interfaces/client_script_interface.hpp"

namespace ts
{
    namespace script
    {
        class Engine;
    }
    
    namespace game
    {
        enum class Script_loader_state
        {
            None,
            Loading,
            Complete
        };

        class Client_script_loader
            : public Generic_loader<Script_loader_state, std::unique_ptr<script_api::Client_interface>>
        {
        public:
            void async_load(std::vector<resources::Script_handle> scripts, const action::Stage* stage, Action_scene* action_scene);

        private:
            std::unique_ptr<script_api::Client_interface> load_scripts(std::vector<resources::Script_handle> scripts, 
                                                                       const action::Stage* stage, Action_scene* action_scene);
        };

        utf8_string to_string(Script_loader_state state);
    }
}

