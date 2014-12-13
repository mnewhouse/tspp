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

#include "audio/audio_store.hpp"

#include "world/world_listener.hpp"

namespace ts
{
    namespace world
    {
        class Entity;
        struct Collision_info;
    }

    namespace audio
    {
        class Sound_effect_controller;
    }

    namespace scene
    {
        class Collision_sound_controller
            : private world::Scoped_world_listener
        {
        public:
            Collision_sound_controller(audio::Sound_effect_controller* effect_controller,
                audio::Audio_handle entity_collision, audio::Audio_handle scenery_collision);

            virtual void on_collision(world::Entity* subject, world::Entity* object,
                const world::Collision_info& collision_info) override;
            
            void play_collision_sound(const world::Entity* subject, const world::Entity* object,
                                      const world::Collision_info& collision_info);

            using Scoped_listener::start_listening;
            using Scoped_listener::stop_listening;

        private:
            audio::Sound_effect_controller* effect_controller_;

            audio::Audio_handle entity_collision_sound_;
            audio::Audio_handle scenery_collision_sound_;
        };
    }
}

#endif