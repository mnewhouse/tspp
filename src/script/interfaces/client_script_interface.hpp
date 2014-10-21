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

#ifndef SCRIPT_CLIENT_INTERFACE_HPP
#define SCRIPT_CLIENT_INTERFACE_HPP

#include "world_interface.hpp"

#include "script/script_engine.hpp"

namespace ts
{
    namespace game
    {
        class Action_scene;
    }

    namespace script_api
    {


        class Client_interface
            : private script::Engine
        {
        public:
            Client_interface(world::World* world, game::Action_scene* action_scene);

            using Engine::create_module;
            using Engine::register_console;

        private:
            World_interface world_interface_;
        };
    }
}

#endif