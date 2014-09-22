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

#pragma once

#ifndef GAME_LOADER_HPP
#define GAME_LOADER_HPP

#include "stage_data.hpp"
#include "action_scene.hpp"

#include "controls/control_center.hpp"
#include "world/world.hpp"

#include "audio/loaded_sound_effects.hpp"
#include "audio/sound_controller.hpp"

#include "resources/resource_store.hpp"

#include "script/script_engine.hpp"
#include "script/interfaces/client_interface.hpp"


namespace ts
{
    namespace game
    {
        struct Loaded_scene
        {
            Loaded_scene() = default;

            Loaded_scene(const Loaded_scene&) = delete;
            Loaded_scene(Loaded_scene&& scene);

            Loaded_scene& operator=(const Loaded_scene&) = delete;
            Loaded_scene& operator=(Loaded_scene&& scene);

            std::unique_ptr<Action_scene> action_scene;
            std::unique_ptr<controls::Control_center> control_center;
            std::unique_ptr<world::World> world;

            std::unique_ptr<audio::Sound_controller> sound_controller;

            std::unique_ptr<script_api::Client_interface> client_script_interface;
        };

        enum class Loading_phase
        {
            Initializing,
            Preprocessing,
            Loading_track_textures,
            Building_track_scene,

            Building_pattern,
            Loading_audio,

            Creating_world,
            Creating_entities,
            Complete
        };

        class Action_loader
        {
        public:
            void async_load(Stage_data stage_data, const resources::Resource_store& resource_store);

            bool complete() const;
            Loaded_scene transfer_loaded_scene();

            double progress() const;
            Loading_phase phase() const;

        private:
            Loaded_scene load_scene(Stage_data stage_data, const resources::Resource_store& resource_store);

            std::future<Loaded_scene> loader_future_;
            std::atomic<double> progress_ = 0.0;
            std::atomic<Loading_phase> phase_ = Loading_phase::Initializing;
        };
    }
}

#endif