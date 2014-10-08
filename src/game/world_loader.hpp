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

#ifndef WORLD_LOADER_HPP
#define WORLD_LOADER_HPP

#include "generic_loader.hpp"

#include "world/world.hpp"

namespace ts
{
    namespace game
    {
        enum class World_loader_state
        {
            None,
            Preprocessing,
            Creating_world,
            Building_pattern,
            Complete
        };

        class World_loader
            : public Generic_loader<World_loader_state, std::unique_ptr<world::World>>
        {
        public:
            World_loader();
            ~World_loader();

            World_loader(const World_loader&) = delete;
            World_loader& operator=(const World_loader&) = delete;

            void async_load(resources::Track_handle track);

            using State = World_loader_state;

        private:
            std::unique_ptr<world::World> load_world(resources::Track_handle track);
        };

        utf8_string to_string(World_loader_state state);
    }
}

#endif