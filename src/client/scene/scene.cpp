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
#include "scene.hpp"

#include "action_scene.hpp"
#include "sound_controller.hpp"

ts::scene::Scene::Scene()
{
}

ts::scene::Scene::~Scene()
{
}

ts::scene::Scene::Scene(Scene&& other)
: action_scene(std::move(other.action_scene)),
  sound_controller(std::move(other.sound_controller))
{
}

ts::scene::Scene& ts::scene::Scene::operator=(Scene&& rhs)
{
    action_scene = std::move(rhs.action_scene);
    sound_controller = std::move(rhs.sound_controller);

    return *this;
}

void ts::scene::Scene::update(std::size_t frame_duration)
{
    if (action_scene)
    {
        action_scene->update(frame_duration);
    }

    if (sound_controller)
    {
        sound_controller->update(frame_duration);
    }
}

void ts::scene::Scene::start()
{
    if (sound_controller)
    {
        sound_controller->start();
    }
}