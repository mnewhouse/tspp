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

#ifndef SCRIPT_WORLD_INTERFACE_HPP
#define SCRIPT_WORLD_INTERFACE_HPP

#include "world/world_listener.hpp"

namespace ts
{
    namespace world
    {
        class World;
    }

    namespace script
    {
        class Engine;
    }

    namespace script_api
    {
        class World_interface
            : public world::World_listener
        {
        public:
            World_interface(script::Engine* engine);

            virtual void on_tick(std::size_t new_ticks) override;
            virtual void on_start() override;

        private:
            script::Engine* engine_;
        };
    }
}

#endif