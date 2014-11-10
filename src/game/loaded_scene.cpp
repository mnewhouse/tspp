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
#include "loaded_scene.hpp"
#include "action_scene.hpp"

#include "audio/sound_controller.hpp"

#include "script/interfaces/client_script_interface.hpp"

ts::game::Loaded_scene::Loaded_scene()
{
}

ts::game::Loaded_scene::~Loaded_scene()
{
}

ts::game::Loaded_scene::Loaded_scene(Loaded_scene&& other)
: action_scene(std::move(other.action_scene)),
  sound_controller(std::move(other.sound_controller)),
  script_interface(std::move(other.script_interface))
{
}

ts::game::Loaded_scene& ts::game::Loaded_scene::operator=(Loaded_scene&& rhs)
{
    action_scene = std::move(rhs.action_scene);
    sound_controller = std::move(rhs.sound_controller);
    script_interface = std::move(rhs.script_interface);

    return *this;
}