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

#ifndef COMMON_CLIENT_HPP
#define COMMON_CLIENT_HPP

#include "client_messages.hpp"
#include "chatbox_interface.hpp"
#include "client_interface.hpp"

#include "scene/loading_sequence.hpp"
#include "scene/scene_interface.hpp"

#include "script/client_script_engine.hpp"

namespace ts
{
    namespace client
    {
        // There are to types of clients; local ones and remote ones. This class encapsulates the
        // common functionality used by both.
        struct Common_logic
        {
            Common_logic(const cup::Cup* cup, resources::Resource_store* resource_store);

            void update(std::size_t frame_duration);
            void on_render();

            std::shared_ptr<scene::Scene> acquire_scene();            

            Message_center message_center_;

            Chatbox_interface chatbox_interface_;
            Client_interface client_interface_;

            Script_engine script_engine_;

            scene::Loading_sequence loading_sequence_;
            scene::Scene_interface scene_interface_;

        private:
            void register_script_apis();
        };       
    }
}

#endif