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

#ifndef GAME_PARTICLE_GENERATOR_HPP
#define GAME_PARTICLE_GENERATOR_HPP

#include "graphics/particle_drawer.hpp"

#include "world/world_listener.hpp"

namespace ts
{
    namespace game
    {
        class Particle_generator
        {
        public:
            void add_car(const world::Car* car);
            void remove_car(const world::Car* car);

            void update(std::size_t ticks_passed);

            void render(sf::RenderTarget& render_target, sf::RenderStates render_states);

        private:
            struct Car_info
            {
                const world::Car* car;
                std::size_t current_tire = 0;
                std::size_t last_ticks = 0;
            };

            graphics::Particle_drawer particle_drawer_;

            std::vector<Car_info> car_list_;
            std::mt19937 random_engine_;
        };
    }
}

#endif