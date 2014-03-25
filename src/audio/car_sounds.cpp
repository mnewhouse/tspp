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


#include "car_sounds.hpp"

#include <SFML/Audio.hpp>

#include "world/car.hpp"
#include "resources/terrain_definition.hpp"

namespace {
    static const double base_pitch = 0.33;
}

void ts::audio::Car_sound_controller::on_car_create(world::Car* car)
{
    const auto& car_definition = car->car_definition();
    auto audio_buffer = audio_store_[car_definition.engine_sample];

    car_sounds_.emplace_back();

    auto& entry = car_sounds_.back();
    entry.car = car;

    if (audio_buffer)
    {
        auto& engine_sound = entry.engine_sound;
        engine_sound = std::make_unique<sf::Sound>(*audio_buffer);
        engine_sound->play();
        engine_sound->setLoop(true);
        engine_sound->setPitch(base_pitch);
        engine_sound->setVolume(base_pitch * 100.0);
    }

    if (skid_sound_)
    {
        auto& skid_sound = entry.skid_sound;
        skid_sound = std::make_unique<sf::Sound>(*skid_sound_);
        skid_sound->setLoop(true);
    }
}

void ts::audio::Car_sound_controller::update(std::size_t ticks)
{
    auto frame_duration = ticks * 0.001;

    for (auto& entry : car_sounds_)
    {
        auto car = entry.car;

        if (auto& engine_sound = entry.engine_sound)
        {
            auto pitch_factor = car->speed() / car->top_speed();
            auto new_pitch = base_pitch + (1.0 - base_pitch) * pitch_factor;

            if (!car->control_state(controls::Control::Accelerate))
            {
                new_pitch = std::max(base_pitch + (new_pitch - base_pitch) * 0.67, engine_sound->getPitch() - frame_duration);
            }

            engine_sound->setPitch(new_pitch);
            engine_sound->setVolume(new_pitch * 100.0);
        }

        if (auto& skid_sound = entry.skid_sound)
        {
            auto traction = car->current_traction();
            const auto& terrain = car->current_terrain();

            auto slide_threshold = car->car_definition().handling.slide_threshold;
            bool is_playing = skid_sound->getStatus() == sf::SoundSource::Playing;

            if (traction < slide_threshold && terrain.roughness == 0.0 && terrain.tire_mark)
            {
                if (!is_playing) skid_sound->play();
            }

            else if (is_playing) skid_sound->stop();
        }
    }
}

void ts::audio::Car_sound_controller::on_entity_destroy(world::Entity* entity)
{
    auto pred = [entity](const Car_sound& sound)
    {
        return sound.car == entity;
    };

    car_sounds_.erase(std::remove_if(car_sounds_.begin(), car_sounds_.end(), pred), car_sounds_.end());
}

void ts::audio::Car_sound_controller::set_skid_sound(Audio_handle skid_sound)
{
    skid_sound_ = std::move(skid_sound);

    for (auto& entry : car_sounds_)
    {
        entry.skid_sound = std::make_unique<sf::Sound>(*skid_sound_);
        entry.skid_sound->setLoop(true);
    }
}

ts::audio::Car_sound_controller::Car_sound::Car_sound(Car_sound&& other)
: car(other.car),
  engine_sound(std::move(other.engine_sound)),
  skid_sound(std::move(other.skid_sound))  
{
}

ts::audio::Car_sound_controller::Car_sound& ts::audio::Car_sound_controller::Car_sound::operator=(Car_sound&& other)
{
    car = other.car;
    engine_sound = std::move(other.engine_sound);
    skid_sound = std::move(other.skid_sound);
    return *this;
}