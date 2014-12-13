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

#ifndef AUDIO_SOUND_EFFECT_CONTROLLER_HPP
#define AUDIO_SOUND_EFFECT_CONTROLLER_HPP

#include "audio_store.hpp"

#include "resources/settings/audio_settings.hpp"

namespace ts
{
    namespace audio
    {
        struct Sound_properties
        {
            float pitch = 1.0;
            float volume = 1.0;
            float attenuation = 1.0;
            float min_distance = 400.0;
            float offset = 0.0;
            Vector2<float> position;
            bool looped = false;
            bool relative_to_listener = false;
        };

        struct Sound_effect_handle
        {
        public:
            Sound_effect_handle() = default;
            explicit operator bool() const;

            void set_position(Vector2<double> position);
            void set_volume(double volume);

        private:
            Sound_effect_handle(std::size_t effect_id_);
            std::size_t effect_id_ = 0;

            friend class Sound_effect_controller;
        };

        class Sound_effect_controller
        {
        public:
            Sound_effect_controller(resources::Audio_settings audio_settings);

            Sound_effect_handle play_sound(const Audio_handle& audio_handle, int priority = 1, Sound_properties properties = {});
            Sound_effect_handle play_looped_sound(const Audio_handle& audio_handle, int priority = 1, Sound_properties properties = {});

            void stop_sound(Sound_effect_handle sound_handle);

            void set_sound_volume(Sound_effect_handle handle, double volume);
            void set_sound_position(Sound_effect_handle handle, Vector2<double> position);

            void update();

        private:
            struct Sound_effect
            {
                Sound_effect(Sound_effect&&);
                Sound_effect();

                Sound_effect& operator=(Sound_effect&& sound_effect);

                std::unique_ptr<sf::Sound> sound;
                int priority = 0;
                std::size_t effect_id = 0;
            };

            std::vector<Sound_effect> sound_queue_;
            resources::Audio_settings audio_settings_;
            std::size_t effect_id_ = 0;
        };
    }
}

#endif