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

#ifndef AUDIO_COLLISION_SOUNDS_HPP
#define AUDIO_COLLISION_SOUNDS_HPP

#include "audio_store.hpp"

#include "world/world_listener.hpp"

namespace ts
{
    namespace audio
    {
        class Sound_effect_controller;

        class Collision_sound_controller
        {
        public:
            Collision_sound_controller(Sound_effect_controller* effect_controller,
                Audio_handle entity_collision, Audio_handle scenery_collision);
            
            void play_collision_sound(const world::Collision_result& collision);

        private:
            Sound_effect_controller* effect_controller_;

            Audio_handle entity_collision_sound_;
            Audio_handle scenery_collision_sound_;
        };
    }
}

#endif