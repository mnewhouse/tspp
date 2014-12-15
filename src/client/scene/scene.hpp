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

#ifndef CLIENT_SCENE_HPP
#define CLIENT_SCENE_HPP

#include "client_script_conductor.hpp"

namespace ts
{
    namespace scene
    {
        class Action_scene;
        struct Sound_controller;

        struct Scene
        {
            Scene();
            ~Scene();

            Scene(Scene&&);
            Scene& operator=(Scene&&);

            void update(std::size_t frame_duration);
            void start();

            std::unique_ptr<Action_scene> action_scene;
            std::unique_ptr<Sound_controller> sound_controller;
            Script_conductor script_conductor;
        };
    }
}

#endif