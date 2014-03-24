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


#include "collision_sounds.hpp"

ts::audio::Collision_sound_controller::Collision_sound_controller()
{
}

void ts::audio::Collision_sound_controller::on_collision(const world::Collision_result& collision)
{
    if (current_ticks_ != last_collision_ticks_)
    {
        sound_list_.erase(std::remove_if(sound_list_.begin(), sound_list_.end(),
            [](const std::unique_ptr<sf::Sound>& sound)
        {
            return sound->getStatus() != sf::SoundSource::Playing;
        }), sound_list_.end());

        const auto& sound_handle = collision.object_state.entity ? entity_collision_sound_ : scenery_collision_sound_;
        if (sound_handle)
        {
            auto sound = std::make_unique<sf::Sound>(*sound_handle);
            sound->setLoop(false);
            sound->play();

            sound_list_.push_back(std::move(sound));
        }


        last_collision_ticks_ = current_ticks_;
    }
}

void ts::audio::Collision_sound_controller::update(std::size_t ticks)
{
    current_ticks_ += ticks;
}

void ts::audio::Collision_sound_controller::set_scenery_collision_sound(const Audio_handle& sound_handle)
{
    scenery_collision_sound_ = sound_handle;
}

void ts::audio::Collision_sound_controller::set_entity_collision_sound(const Audio_handle& sound_handle)
{
    entity_collision_sound_ = sound_handle;
}
