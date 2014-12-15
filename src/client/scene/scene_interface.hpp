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

#ifndef SCENE_INTERFACE_HPP
#define SCENE_INTERFACE_HPP

namespace ts
{
    namespace scene
    {
        class Action_scene;
        struct Sound_controller;
        class Script_conductor;
        struct Scene;

        struct Scene_interface
        {
        public:
            Scene_interface(std::shared_ptr<Scene> scene_ptr = {});
            ~Scene_interface();

            void reset(std::shared_ptr<Scene> scene_ptr);

            Action_scene* action_scene() const;
            Sound_controller* sound_controller() const;
             Script_conductor* script_conductor() const;

        private:
            std::weak_ptr<Scene> scene_ptr_;
        };
    };
}

#endif