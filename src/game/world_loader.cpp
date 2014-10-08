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
#include "world_loader.hpp"

#include "resources/pattern_builder.hpp"

#include "world/world.hpp"

ts::game::World_loader::World_loader()
{
}

ts::game::World_loader::~World_loader()
{
}

void ts::game::World_loader::async_load(resources::Track_handle track)
{
    auto func = [=]()
    {
        return load_world(track);
    };

    set_state(World_loader_state::None);
    Generic_loader::async_load(func);
}

std::unique_ptr<ts::world::World> ts::game::World_loader::load_world(resources::Track_handle track_handle)
{
    set_state(State::Preprocessing);
    auto track = std::make_unique<resources::Track>(track_handle);
    std::size_t sub_tile_count = track->placed_tiles().size();

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

    set_state(State::Complete);
    return world_ptr;
}

ts::utf8_string ts::game::to_string(World_loader_state state)
{
    switch (state)
    {
    case World_loader_state::Preprocessing:
        return "Preprocessing...";

    case World_loader_state::Building_pattern:
        return "Building pattern...";

    case World_loader_state::Creating_world:
        return "Creating world...";

    default:
        return "";

    }
}