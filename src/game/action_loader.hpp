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

#include "action_scene.hpp"
#include "generic_loader.hpp"

namespace ts
{
    namespace resources
    {
        struct Resource_store;
    }

    namespace world
    {
        class World;
    }
    
    namespace action
    {
        class Stage;
    }

    namespace game
    {
        class Action_scene;
        class Car_image_generator;

        enum class Action_loader_state
        {
            None,
            Loading_track_textures,
            Building_track_scene,
            Loading_entity_textures,
        };

        class Action_loader
            : public Generic_loader<Action_loader_state, std::unique_ptr<Action_scene>>
        {
        public:
            void async_load(const action::Stage* stage, const resources::Video_settings& video_settings);
                            

            using State = Action_loader_state;

        private:
            std::unique_ptr<Action_scene> load_scene(const action::Stage* stage, const resources::Video_settings& video_settings);
                                                     
        };

        utf8_string to_string(Action_loader_state state);
    }
}

#endif