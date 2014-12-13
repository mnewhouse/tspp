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
#include "skid_sounds.hpp"

#include "world/car.hpp"

#include "resources/terrain_definition.hpp"

namespace ts
{
    namespace scene
    {
        static double calculate_skid_volume(double speed, double traction)
        {
            return std::min(speed / 400.0 / std::max(traction, 0.5), 1.0);
        }
    }
}

ts::scene::Skid_sound_controller::Skid_sound_controller(audio::Sound_effect_controller* sound_effect_controller, 
                                                        audio::Audio_handle skid_sound)
: sound_effect_controller_(sound_effect_controller),
  skid_sound_(std::move(skid_sound))
{
}

void ts::scene::Skid_sound_controller::register_car(const world::Car* car)
{
    skid_data_.emplace_back();

    auto& entry = skid_data_.back();
    entry.car = car;
}

void ts::scene::Skid_sound_controller::unregister_car(const world::Car* car)
{
    auto matching_car = [car](const Skid_data& skid_data)
    {
        return skid_data.car == car;
    };

    auto it = std::find_if(skid_data_.begin(), skid_data_.end(), matching_car);
    if (it != skid_data_.end())
    {
        skid_data_.erase(it);
    }
}

void ts::scene::Skid_sound_controller::update()
{
    for (auto& data : skid_data_)
    {
        auto car = data.car;
        const auto& car_def = car->car_definition();
        const auto& terrain = car->current_terrain();
        
        auto speed = car->speed();
        auto slide_threshold = car_def.handling->slide_threshold;        
        auto traction = car->current_traction();        

        bool skid_state = (speed != 0.0 && traction < slide_threshold && terrain.skid_mark);
        bool prev_state = static_cast<bool>(data.sound_effect);

        if (skid_state != prev_state)
        {
            if (skid_state)
            {
                int priority = car->locality() == world::Locality::Local ? 3 : 1;

                audio::Sound_properties properties;
                properties.position = car->position();
                properties.min_distance = 100.0;
                properties.attenuation = 1.0;
                properties.volume = static_cast<float>(calculate_skid_volume(car->speed(), traction));
                properties.relative_to_listener = false;

                data.sound_effect = sound_effect_controller_->play_looped_sound(skid_sound_, priority, properties);
            }

            else
            {
                sound_effect_controller_->stop_sound(data.sound_effect);
                data.sound_effect = {};
            }
        }

        else
        {
            sound_effect_controller_->set_sound_position(data.sound_effect, car->position());
            sound_effect_controller_->set_sound_volume(data.sound_effect, calculate_skid_volume(car->speed(), traction));
        }
    }
}