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
#include "client_script_loader.hpp"

#include "script/script_engine.hpp"
#include "script/script_module.hpp"


void ts::game::Client_script_loader::async_load(std::vector<resources::Script_handle> scripts, const action::Stage* stage, Action_scene* action_scene)
{
    auto callable = [=]()
    {
        return load_scripts(scripts, stage, action_scene);
    };

    Generic_loader::async_load(callable);
}

std::unique_ptr<ts::script_api::Client_interface> ts::game::Client_script_loader::load_scripts(std::vector<resources::Script_handle> scripts, 
                                                                                               const action::Stage* stage, Action_scene* action_scene)
{
    set_state(Script_loader_state::Loading);
    auto interface = std::make_unique<script_api::Client_interface>(stage, action_scene);

    for (const auto& script_handle : scripts)
    {
        auto module = interface->create_module(script_handle->name());

        for (const auto& file_name : script_handle->client_scripts())
        {
            module->do_file(file_name);
        }
    }

    set_state(Script_loader_state::Complete);

    return interface;
}

ts::utf8_string ts::game::to_string(Script_loader_state state)
{
    switch (state)
    {
    case Script_loader_state::Loading:
        return "Loading scripts...";

    default:
        return "";
    }
}