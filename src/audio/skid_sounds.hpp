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

#ifndef AUDIO_SKID_SOUNDS_HPP
#define AUDIO_SKID_SOUNDS_HPP

#include "audio_store.hpp"
#include "sound_effect_controller.hpp"

namespace ts
{
    namespace world
    {
        class Car;
    }

    namespace audio
    {
        class Sound_effect_controller;

        class Skid_sound_controller
        {
        public:
            Skid_sound_controller(Sound_effect_controller* sound_effect_controller, Audio_handle skid_sound);

            void register_car(const world::Car* car);
            void unregister_car(const world::Car* car);

            void update();

        private:
            Sound_effect_controller* sound_effect_controller_;
            Audio_handle skid_sound_;

            struct Skid_data
            {
                const world::Car* car;
                Sound_effect_handle sound_effect;
            };

            std::vector<Skid_data> skid_data_;
        };
    }
}

#endif