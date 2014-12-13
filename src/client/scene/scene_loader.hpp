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

#ifndef SCENE_LOADER_HPP
#define SCENE_LOADER_HPP

#include "action_scene.hpp"
#include "resources/generic_loader.hpp"

namespace ts
{
    namespace resources
    {
        struct Resource_store;
    }
    
    namespace action
    {
        class Stage;
    }

    namespace scene
    {
        class Action_scene;
        class Car_image_generator;

        enum class Scene_loader_state
        {
            None,
            Loading_track_textures,
            Building_track_scene,
            Loading_entity_textures,
        };

        class Scene_loader
            : public resources::Generic_loader<Scene_loader_state, std::unique_ptr<Action_scene>>
        {
        public:
            void async_load(const action::Stage* stage, const resources::Video_settings& video_settings);
                            

            using State = Scene_loader_state;

        private:
            std::unique_ptr<Action_scene> load_scene(const action::Stage* stage, const resources::Video_settings& video_settings);
                                                     
        };

        utf8_string to_string(Scene_loader_state state);
    }
}

#endif