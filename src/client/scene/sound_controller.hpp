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

#ifndef AUDIO_SOUND_CONTROLLER_HPP
#define AUDIO_SOUND_CONTROLLER_HPP

#include "audio/sound_effect_controller.hpp"

#include "engine_sounds.hpp"
#include "collision_sounds.hpp"
#include "skid_sounds.hpp"
#include "loaded_sound_effects.hpp"

namespace ts
{
    namespace scene
    {
        struct Sound_controller
        {
            Sound_controller(resources::Audio_settings audio_settings, audio::Loaded_sound_effects sound_effects);

            void update(std::size_t frame_duration);
            void start();

            audio::Sound_effect_controller sound_effect_controller;

            Engine_sound_controller engine_sounds;
            Collision_sound_controller collision_sounds;
            Skid_sound_controller skid_sounds;
        };
    }
}

#endif