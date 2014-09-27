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

namespace ts
{
    namespace controls
    {
        class Control_center;
    }

    namespace world
    {
        class World;
    }

    namespace audio
    {
        struct Sound_controller;
    }

    namespace script_api
    {
        class Client_interface;
    }

    namespace resources
    {
        struct Resource_store;
    }

    namespace game
    {
        class Action_scene;

        struct Loaded_scene
        {
            Loaded_scene();
            ~Loaded_scene();

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
            None,
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
            Action_loader();
            ~Action_loader();

            void async_load(Stage_data stage_data, const resources::Resource_store& resource_store);

            bool complete() const;
            Loaded_scene transfer_loaded_scene();

            double progress() const;
            Loading_phase phase() const;

            bool is_loading() const;

        private:
            Loaded_scene load_scene(Stage_data stage_data, const resources::Resource_store& resource_store);

            std::future<Loaded_scene> loader_future_;
            std::atomic<double> progress_ = 0.0;
            std::atomic<Loading_phase> phase_ = Loading_phase::None;
        };
    }
}

#endif