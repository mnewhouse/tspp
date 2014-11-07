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
#include "engine_sounds.hpp"

#include "world/car.hpp"
#include "resources/terrain_definition.hpp"

#include <SFML/Audio.hpp>

namespace 
{
    static const float base_pitch = 0.33f;
}

ts::audio::Engine_sound_controller::Engine_sound_controller(resources::Audio_settings audio_settings, Audio_store engine_sounds)
    : audio_settings_(audio_settings),
      engine_sound_store_(engine_sounds)
{
}

void ts::audio::Engine_sound_controller::register_car(const world::Car* car)
{
    const auto& car_definition = car->car_definition();
    auto audio_buffer = engine_sound_store_.load_from_file(car_definition.engine_sample);

    engine_sounds_.push_back({ car, nullptr });
    if (audio_buffer)
    {
        auto& entry = engine_sounds_.back();
        auto sound_ptr = std::make_unique<sf::Sound>();
        sound_ptr->setLoop(true);
        sound_ptr->setPitch(base_pitch);
        sound_ptr->setBuffer(*audio_buffer);

        entry.second = std::move(sound_ptr);

        sorting_required_ = true;
    }
}

void ts::audio::Engine_sound_controller::sort_sounds_by_locality()
{
    auto locality_sort = [](const Engine_sound& a, const Engine_sound& b)
    {
        auto a_loc = a.first->locality();
        auto b_loc = b.first->locality();

        if (a_loc == b_loc) return false;

        return a_loc == world::Locality::Local;    
    };

    std::sort(engine_sounds_.begin(), engine_sounds_.end(), locality_sort);

    sorting_required_ = false;
}

void ts::audio::Engine_sound_controller::play_prioritized_sounds()
{
    std::size_t index = 0;
    for (; index != audio_settings_.engine_channels && index != engine_sounds_.size(); ++index)
    {
        if (auto& ptr = engine_sounds_[index].second)
        {
            ptr->play();
        }
    }

    for (; index != engine_sounds_.size(); ++index)
    {
        if (auto& ptr = engine_sounds_[index].second)
        {
            ptr->stop();
        }
    }
}

void ts::audio::Engine_sound_controller::update(std::size_t ticks)
{
    auto frame_duration = ticks * 0.001f;

    if (sorting_required_ && started_)
    {
        sort_sounds_by_locality();
        play_prioritized_sounds();
    }

    for (auto& entry : engine_sounds_)
    {
        auto car = entry.first;

        if (auto& engine_sound = entry.second)
        {            
            auto speed = car->speed(), top_speed = car->top_speed();
            if (top_speed == 0.0) continue;

            const float pitch_factor = std::min(static_cast<float>(speed / top_speed), 1.1f);
            float new_pitch = base_pitch + (1.0f - base_pitch) * pitch_factor;

            if (!car->control_state(controls::Control::Accelerate))
            {
                new_pitch = std::max(base_pitch + (new_pitch - base_pitch) * 0.67f, static_cast<float>(engine_sound->getPitch()) - frame_duration);
            }

            engine_sound->setPitch(new_pitch);
            engine_sound->setVolume(new_pitch * new_pitch * 100.0f);
        }
    }
}

void ts::audio::Engine_sound_controller::unregister_car(const world::Car* car)
{
    auto find_car = [car](const Engine_sound& sound)
    {
        return sound.first == car;
    };

    engine_sounds_.erase(std::remove_if(engine_sounds_.begin(), engine_sounds_.end(), find_car), engine_sounds_.end());
}


void ts::audio::Engine_sound_controller::start()
{
    started_ = true;
}