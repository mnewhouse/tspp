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

#ifndef AUDIO_ENGINE_SOUNDS_HPP
#define AUDIO_ENGINE_SOUNDS_HPP

#include "audio_store.hpp"

#include "resources/settings/audio_settings.hpp"

namespace ts
{
    namespace world
    {
        class Car;
    }

    namespace audio
    {
        class Engine_sound_controller
        {
        public:
            Engine_sound_controller(resources::Audio_settings audio_settings, Audio_store engine_sound_store);

            void update(std::size_t ticks);
            void register_car(const world::Car* car);
            void unregister_car(const world::Car* car);

            void start();

        private:
            void sort_sounds_by_locality();
            void play_prioritized_sounds();

            using Engine_sound = std::pair<const world::Car*, std::unique_ptr<sf::Sound>>;

            std::vector<Engine_sound> engine_sounds_;

            Audio_store engine_sound_store_;
            resources::Audio_settings audio_settings_;
            bool sorting_required_ = false;
            bool started_ = false;
        };
    }
}

#endif