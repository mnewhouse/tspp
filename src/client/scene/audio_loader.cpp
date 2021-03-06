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
#include "audio_loader.hpp"

#include "action/stage.hpp"

#include "world/world.hpp"

void ts::scene::Audio_loader::async_load(const action::Stage* stage, const resources::Audio_settings& audio_settings)
{
    auto callable = [=]()
    {
        return load_audio(stage, audio_settings);
    };

    Generic_loader::async_load(callable);
}

std::unique_ptr<ts::scene::Sound_controller> ts::scene::Audio_loader::load_audio(const action::Stage* stage, const resources::Audio_settings& audio_settings)
{
    audio::Loaded_sound_effects sound_effects;

    std::set<utf8_string> needed_engine_sounds;
    for (const auto& car_info : stage->car_data())
    {
        if (auto model = car_info.car_def.model)
        {
            needed_engine_sounds.insert(model->engine_sample);
        }        
    }

    for (const auto& engine_sample : needed_engine_sounds)
    {
        sound_effects.engine_sounds.load_from_file(engine_sample);
    }

    audio::Audio_store audio_store;
    sound_effects.entity_collision = audio_store.load_from_file("sound/carcollision.wav");
    sound_effects.scenery_collision = audio_store.load_from_file("sound/collision.wav");
    sound_effects.skid_sound = audio_store.load_from_file("sound/skid.wav");

    auto sound_controller = std::make_unique<Sound_controller>(audio_settings, sound_effects);
    sound_controller->collision_sounds.start_listening(stage->world_listener_host());

    for (const auto& car_info : stage->car_data())
    {
        const auto car = car_info.car;
        sound_controller->engine_sounds.register_car(car);
        sound_controller->skid_sounds.register_car(car);
    }
    
    return sound_controller;
}

ts::utf8_string ts::scene::to_string(Audio_loader_state state)
{
    switch (state)
    {
    case Audio_loader_state::Loading_audio:
        return "Loading audio...";

    default:
        return "";
    }
}