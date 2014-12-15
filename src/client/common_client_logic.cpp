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
#include "common_client_logic.hpp"

#include "script/client_script_api.hpp"
#include "script/client_core_events.hpp"

ts::client::Common_logic::Common_logic(const cup::Cup* cup, resources::Resource_store* resource_store)
    : message_center_(),
      chatbox_interface_(&message_center_),
      client_interface_(&message_center_, cup, chatbox_interface_.chatbox(), resource_store),
      script_engine_(),
      loading_sequence_(resource_store, script_engine_.engine()),
      scene_interface_()
{
    register_script_apis();
}

void ts::client::Common_logic::update(std::size_t frame_duration)
{
    loading_sequence_.poll();
}

void ts::client::Common_logic::on_render()
{
    script_events::on_render(script_engine_);
}

std::shared_ptr<ts::scene::Scene> ts::client::Common_logic::acquire_scene()
{
    auto shared_scene = std::make_shared<scene::Scene>(loading_sequence_.transfer_result());
    scene_interface_.reset(shared_scene);
    return shared_scene;
}

void ts::client::Common_logic::register_script_apis()
{
    auto engine = script_engine_.engine();
    script_api::register_core_api(engine);
    script_api::register_event_api(engine, &script_engine_);
    script_api::register_scene_api(engine, &scene_interface_);
    script_api::register_stage_api(engine, nullptr);

    script_api::register_stdout_console(engine);    
}