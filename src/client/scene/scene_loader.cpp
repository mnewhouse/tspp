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

#include "scene_loader.hpp"
#include "action_scene.hpp"
#include "track_builder.hpp"

#include "action/stage.hpp"
#include "world/world.hpp"
#include "world/car.hpp"

#include "resources/settings/video_settings.hpp"
#include "resources/car_image_generator.hpp"

#include "graphics/texture.hpp"


void ts::scene::Scene_loader::async_load(const action::Stage* stage, const resources::Video_settings& video_settings)
{
    set_state(State::None);

    auto callable = [=]()
    {
        return load_scene(stage, video_settings);
    };

    Generic_loader::async_load(callable);
}


std::unique_ptr<ts::scene::Action_scene> ts::scene::Scene_loader::load_scene(const action::Stage* stage, const resources::Video_settings& video_settings)
{
    double progress_step = 0.0;
    auto increment_progress = [this, &progress_step]()
    {
        set_progress(progress() + progress_step);
    };

    using resources::Tile_group_definition;
    using resources::Level_tile;
    using resources::Tile_definition;

    std::set<utf8_string> needed_textures;
    std::set<utf8_string> needed_pattern_files;

    const auto& world = stage->world();
    const auto& track = world.track();
    const auto& car_list = stage->car_data();

    std::set<utf8_string> needed_engine_sounds;

    for (const auto& car_data : car_list)
    {
        if (auto model = car_data.car_def.model)
        {
            needed_engine_sounds.insert(model->engine_sample);
        }        
    }

    Track_builder track_builder(track);

    set_state(State::Loading_track_textures);
    progress_step = 1.0 / needed_textures.size();

    for (const auto& texture_path : needed_textures)
    {
        track_builder.preload_image(texture_path);

        increment_progress();
    }

    set_state(State::Building_track_scene);
    set_progress(0.0);
    progress_step = 1.0 / track_builder.step_count();

    auto track_scene = track_builder(increment_progress);
    auto action_scene = std::make_unique<Action_scene>(std::move(track_scene), video_settings);

    set_progress(0.0);
    set_state(State::Loading_entity_textures);

    resources::Car_image_generator car_image_generator;

    std::size_t controlled_car_count = 0;
    for (const auto& car_data : car_list)
    {
        const auto car_model = car_data.car_def.model;
        if (!car_model) continue;

        auto image = car_image_generator(*car_model, car_data.player.color);

        Drawable_entity_definition entity_def(std::make_shared<graphics::Texture>(image));
        entity_def.image_type = car_model->image_type;
        entity_def.texture_rect = car_model->image_rect;
        entity_def.scale.x = car_model->image_scale;
        entity_def.scale.y = car_model->image_scale;

        action_scene->add_car(car_data.car, entity_def);
        if (stage->is_car_controlled(car_data.car))
        {
            // Spectate all cars that we control.
            action_scene->add_followed_entity(car_data.car);
            ++controlled_car_count;
        }
    }

    // If we are not controlling any cars locally, we should at least spectate one.
    if (controlled_car_count == 0 && !car_list.empty())
    {
        action_scene->add_followed_entity(car_list.front().car);
    }

    set_progress(1.0);

    return action_scene;
}


ts::utf8_string ts::scene::to_string(Scene_loader_state state)
{
    switch (state)
    {
    case Scene_loader_state::Building_track_scene:
        return "Building track scene...";

    case Scene_loader_state::Loading_entity_textures:
    case Scene_loader_state::Loading_track_textures:
        return "Loading textures...";

    default:
        return "";
    }
}