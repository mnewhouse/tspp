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
#include "sound_controller.hpp"


ts::audio::Sound_controller::Sound_controller(resources::Audio_settings audio_settings, audio::Loaded_sound_effects sound_effects)
: sound_effect_controller(audio_settings),
  engine_sounds(audio_settings, sound_effects.engine_sounds),
  collision_sounds(&sound_effect_controller, sound_effects.entity_collision, sound_effects.scenery_collision),
  skid_sounds(&sound_effect_controller, sound_effects.skid_sound)
{
}

void ts::audio::Sound_controller::update(std::size_t frame_duration)
{
    engine_sounds.update(frame_duration);
    skid_sounds.update();

    sound_effect_controller.update();
}

