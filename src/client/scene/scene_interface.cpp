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
#include "scene_interface.hpp"
#include "scene.hpp"

ts::scene::Scene_interface::Scene_interface(std::shared_ptr<Scene> scene_ptr)
    : scene_ptr_(std::move(scene_ptr))
{
}

ts::scene::Scene_interface::~Scene_interface()
{
}

void ts::scene::Scene_interface::reset(std::shared_ptr<Scene> scene_ptr)
{
    scene_ptr_ = std::move(scene_ptr);
}

ts::scene::Action_scene* ts::scene::Scene_interface::action_scene() const
{
    if (auto scene = scene_ptr_.lock())
    {
        return scene->action_scene.get();
    }

    return nullptr;
}

ts::scene::Sound_controller* ts::scene::Scene_interface::sound_controller() const
{
    if (auto scene = scene_ptr_.lock())
    {
        return scene->sound_controller.get();
    }

    return nullptr;
}