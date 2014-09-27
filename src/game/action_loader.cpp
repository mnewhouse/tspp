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
#include "action_loader.hpp"
#include "action_scene.hpp"

#include "game/track_builder.hpp"

#include "controls/control_center.hpp"

#include "audio/loaded_sound_effects.hpp"
#include "audio/sound_controller.hpp"

#include "resources/pattern_builder.hpp"
#include "resources/resource_store.hpp"

#include "script/script_engine.hpp"
#include "script/interfaces/client_interface.hpp"

#include "world/world.hpp"

ts::game::Action_loader::Action_loader()
{
}

ts::game::Action_loader::~Action_loader()
{
}

void ts::game::Action_loader::async_load(Stage_data stage_data, const resources::Resource_store& resource_store)
{
    phase_ = Loading_phase::Initializing;

    auto callable = [this](Stage_data stage_data, const resources::Resource_store& resource_store)
    {
        return load_scene(std::move(stage_data), resource_store);
    };

    loader_future_ = std::async(std::launch::async, callable, std::move(stage_data), resource_store);
}

ts::game::Loaded_scene ts::game::Action_loader::transfer_loaded_scene()
{
    phase_ = Loading_phase::None;
    return loader_future_.get();
}

double ts::game::Action_loader::progress() const
{
    return progress_;
}

ts::game::Loading_phase ts::game::Action_loader::phase() const
{
    return phase_;
}

bool ts::game::Action_loader::complete() const
{
    return phase_ == Loading_phase::Complete;
}

bool ts::game::Action_loader::is_loading() const
{
    return phase_ != Loading_phase::None;
}

ts::game::Loaded_scene ts::game::Action_loader::load_scene(Stage_data stage_data, const resources::Resource_store& resource_store)
{
    const auto& settings = resource_store.settings;

    phase_ = Loading_phase::Preprocessing;
    progress_ = 0.0;

    double progress_step = 0.0;
    auto increment_progress = [this, &progress_step]()
    {
        progress_ = progress_ + progress_step;
    };

    resources::Track track(stage_data.track_handle);

    using resources::Tile_group_definition;
    using resources::Level_tile;
    using resources::Tile_definition;

    std::set<utf8_string> needed_textures;
    std::set<utf8_string> needed_pattern_files;

    std::size_t sub_tile_count = track.placed_tiles().size();

    std::set<utf8_string> needed_engine_sounds;

    for (auto& player_data : stage_data.cars)
    {
        if (auto car = player_data.car)
        {
            needed_engine_sounds.insert(car->engine_sample);
        }
    }

    game::Track_builder track_builder(track);

    phase_ = Loading_phase::Loading_track_textures;
    progress_step = 1.0 / needed_textures.size();

    for (const auto& texture_path : needed_textures)
    {
        track_builder.preload_image(texture_path);

        increment_progress();
    }

    phase_ = Loading_phase::Building_track_scene;
    progress_ = 0.0;
    progress_step = 1.0 / track_builder.step_count();

    Loaded_scene loaded_scene;
    auto track_scene = track_builder(increment_progress);

    phase_ = Loading_phase::Building_pattern;
    progress_ = 0.0;
    progress_step = 0.5 / needed_pattern_files.size();

    resources::Pattern_builder pattern_builder(track);


    for (const auto& pattern_file : needed_pattern_files)
    {
        pattern_builder.preload_pattern(pattern_file);

        increment_progress();
    }

    progress_step = 0.5 / sub_tile_count;
    auto track_pattern = pattern_builder(increment_progress);

    loaded_scene.control_center = std::make_unique<controls::Control_center>();

    progress_ = 0.0;
    phase_ = Loading_phase::Creating_world;    
    loaded_scene.world = std::make_unique<world::World>(std::move(track), std::move(track_pattern));

    struct Car_info
    {
        const world::Car* car;
        resources::Player_color color;
        controls::Slot control_slot;
    };

    std::vector<Car_info> car_list;

    progress_step = 1.0 / stage_data.cars.size();
    for (auto& car_data : stage_data.cars)
    {
        if (auto car = loaded_scene.world->create_car(*car_data.car, car_data.start_pos))
        {
            if (car_data.control_slot != controls::invalid_slot)
            {
                loaded_scene.control_center->assume_control(car_data.control_slot, car);
            }

            car_list.emplace_back();
            car_list.back().car = car;
            car_list.back().color = car_data.color;
            car_list.back().control_slot = car_data.control_slot;
        }

        increment_progress();
    }

    auto current_resolution = settings.video_settings.current_screen_resolution;
    loaded_scene.action_scene = std::make_unique<Action_scene>(std::move(track_scene), settings.video_settings);

    progress_ = 0.0;
    phase_ = Loading_phase::Creating_entities;
    progress_step = 1.0 / loaded_scene.world->car_list().size();

    for (auto& car_info : car_list)
    {
        loaded_scene.action_scene->add_car(car_info.car, car_info.color);

        if (car_info.control_slot != controls::invalid_slot)
        {
            loaded_scene.action_scene->add_followed_entity(car_info.car);
        }
    }

    progress_ = 0.0;
    phase_ = Loading_phase::Loading_audio;
    progress_step = 1.0 / (needed_engine_sounds.size() + 3);

    audio::Loaded_sound_effects sound_effects;

    for (auto& sound_file : needed_engine_sounds)
    {
        sound_effects.engine_sounds.load_from_file(sound_file);
        increment_progress();
    }

    audio::Audio_store audio_store;
    sound_effects.entity_collision = audio_store.load_from_file("sound/carcollision.wav");
    increment_progress();

    sound_effects.scenery_collision = audio_store.load_from_file("sound/collision.wav");
    increment_progress();

    sound_effects.skid_sound = audio_store.load_from_file("sound/skid.wav");
    increment_progress();

    loaded_scene.sound_controller = std::make_unique<audio::Sound_controller>(settings.audio_settings, sound_effects);

    loaded_scene.client_script_interface = std::make_unique<script_api::Client_interface>(&*loaded_scene.world,
                                                                                          &*loaded_scene.action_scene);
    loaded_scene.client_script_interface->register_console(script::Stdout_console());

    for (const auto& script : stage_data.loaded_scripts)
    {
        auto module = loaded_scene.client_script_interface->create_module(script->name());

        for (const auto& script_file : script->client_scripts())
        {
            module->do_file(script_file);
        }
    }


    auto& engine_sounds = loaded_scene.sound_controller->engine_sounds;
    auto& skid_sounds = loaded_scene.sound_controller->skid_sounds;
    for (auto& car : loaded_scene.world->car_list())
    {
        engine_sounds.register_car(&*car);
        skid_sounds.register_car(&*car);
    }

    progress_ = 1.0;
    phase_ = Loading_phase::Complete;

    return loaded_scene;
}

ts::game::Loaded_scene::Loaded_scene()
{
}

ts::game::Loaded_scene::~Loaded_scene()
{
}

ts::game::Loaded_scene::Loaded_scene(Loaded_scene&& other)
: action_scene(std::move(other.action_scene)),
control_center(std::move(other.control_center)),
world(std::move(other.world)),
sound_controller(std::move(other.sound_controller)),
client_script_interface(std::move(other.client_script_interface))
{
}

ts::game::Loaded_scene& ts::game::Loaded_scene::operator=(Loaded_scene&& rhs)
{
    action_scene = std::move(rhs.action_scene);
    control_center = std::move(rhs.control_center);
    world = std::move(rhs.world);
    sound_controller = std::move(rhs.sound_controller);
    client_script_interface = std::move(rhs.client_script_interface);

    return *this;
}