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
#include "collision_sounds.hpp"
#include "sound_effect_controller.hpp"

#include "world/collisions.hpp"


ts::audio::Collision_sound_controller::Collision_sound_controller(Sound_effect_controller* effect_controller,
    Audio_handle entity_collision, Audio_handle scenery_collision)
: effect_controller_(effect_controller),
  entity_collision_sound_(std::move(entity_collision)),
  scenery_collision_sound_(std::move(scenery_collision))
{
}

void ts::audio::Collision_sound_controller::play_collision_sound(const world::Collision_result& collision)
{
    auto subject = collision.subject_state.entity;
    auto object = collision.object_state.entity;

    const auto& sound_handle = object ? entity_collision_sound_ : scenery_collision_sound_;
    int priority = 1; 
    if (subject->locality() == world::Locality::Local || (object && object->locality() == world::Locality::Local))
    {
        priority = 2;
    }

    Sound_properties properties;
    properties.position = collision.global_point;
    properties.volume = std::min(static_cast<float>(collision.impact), 200.0f) / 200.0f;

    if (sound_handle)
    {
        effect_controller_->play_sound(sound_handle, priority, properties);
    }    
}
