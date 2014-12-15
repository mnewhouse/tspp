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
#include "client_script_api.hpp"

#include "script/script_engine.hpp"

#include "script/api/client/view_api.hpp"

namespace ts
{
    namespace client
    {
        namespace script_api
        {
            using namespace ts::script_api;
        }
    }
}

void ts::client::script_api::register_scene_api(script::Engine* engine, const scene::Scene_interface* scene_interface)
{
    engine->register_api(script_api::view_api(scene_interface));
}