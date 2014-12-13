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

#include "audio/sound_effect_controller.hpp"

#include "world/collisions.hpp"


ts::scene::Collision_sound_controller::Collision_sound_controller(audio::Sound_effect_controller* effect_controller,
    audio::Audio_handle entity_collision, audio::Audio_handle scenery_collision)
: effect_controller_(effect_controller),
  entity_collision_sound_(std::move(entity_collision)),
  scenery_collision_sound_(std::move(scenery_collision))
{
}

void ts::scene::Collision_sound_controller::on_collision(world::Entity* subject, world::Entity* object,
                                                         const world::Collision_info& collision_info)
{
    play_collision_sound(subject, object, collision_info);
}

void ts::scene::Collision_sound_controller::play_collision_sound(const world::Entity* subject, const world::Entity* object, const world::Collision_info& collision_info)
{
    const auto& sound_handle = object ? entity_collision_sound_ : scenery_collision_sound_;
    std::int32_t priority = 1; 
    if (subject->locality() == world::Locality::Local || (object && object->locality() == world::Locality::Local))
    {
        priority = 2;
    }

    audio::Sound_properties properties;
    properties.position = collision_info.point;
    properties.volume = std::min(static_cast<float>(collision_info.impact), 200.0f) / 200.0f;
    properties.attenuation = 0.0;

    if (sound_handle)
    {
        effect_controller_->play_sound(sound_handle, priority, properties);
    }    
}
