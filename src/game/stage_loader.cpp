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
#include "stage_loader.hpp"

#include "cup/stage_data.hpp"

#include "resources/pattern_builder.hpp"

#include "world/world.hpp"

ts::game::Stage_loader::Stage_loader()
{
}

ts::game::Stage_loader::~Stage_loader()
{
}

void ts::game::Stage_loader::async_load(const action::Stage_data& stage_data, Script_loader_function script_loader)
{
    auto func = [=]()
    {
        return load_stage(stage_data, script_loader);
    };

    set_state(Stage_loader_state::None);
    Generic_loader::async_load(func);
}

std::unique_ptr<ts::action::Stage> ts::game::Stage_loader::load_stage(const action::Stage_data& stage_data, const Script_loader_function& script_loader)
{
    set_state(State::Preprocessing);
    auto track = std::make_unique<resources::Track>(stage_data.track);
    std::size_t sub_tile_count = track->placed_tile_count();

    double progress_step = 1.0 / sub_tile_count;
    auto increment_progress = [&progress_step, this]()
    {
        set_progress(progress() + progress_step);
    };
    
    set_state(State::Building_pattern);
    resources::Pattern_builder pattern_builder(*track);
    auto track_pattern = pattern_builder(increment_progress);

    set_state(State::Creating_world);
    auto world_ptr = std::make_unique<world::World>(std::move(track), std::move(track_pattern));

    set_state(State::Creating_entities);
    auto stage = std::make_unique<action::Stage>(std::move(world_ptr), stage_data);

    if (script_loader)
    {
        set_state(State::Loading_scripts);
        script_loader(stage.get());
    }

    set_state(State::Complete);
    return stage;
}

ts::utf8_string ts::game::to_string(Stage_loader_state state)
{
    switch (state)
    {
    case Stage_loader_state::Preprocessing:
        return "Preprocessing...";

    case Stage_loader_state::Building_pattern:
        return "Building pattern...";

    case Stage_loader_state::Creating_world:
        return "Creating world...";

    case Stage_loader_state::Creating_entities:
        return "Creating entities...";

    case Stage_loader_state::Loading_scripts:
        return "Loading scripts...";

    default:
        return "";

    }
}