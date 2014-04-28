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

#include <vector>

namespace ts
{
    namespace audio
    {
        class Collision_sound_controller
            : public world::World_listener
        {
        public:
            Collision_sound_controller();
            
            virtual void on_collision(const world::Collision_result& collision) override;

            void update(std::size_t ticks);

            void set_entity_collision_sound(const Audio_handle& sound_handle);
            void set_scenery_collision_sound(const Audio_handle& sound_handle);

        private:
            std::vector<std::unique_ptr<sf::Sound>> sound_list_;

            Audio_handle entity_collision_sound_;
            Audio_handle scenery_collision_sound_;

            std::size_t current_ticks_ = 0;
            std::size_t last_collision_ticks_ = 0;
        };
    }
}

#endif