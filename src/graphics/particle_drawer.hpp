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

#ifndef GRAPHICS_PARTICLE_DRAWER
#define GRAPHICS_PARTICLE_DRAWER

#include "render_scene.hpp"

namespace ts
{
    namespace graphics
    {
        class Particle_drawer
        {
        public:
            void create_particle(Vector2<double> position, double radius, sf::Color color, std::size_t ticks_visible);

            void render(sf::RenderTarget& render_target, sf::RenderStates render_states);

            void update(std::size_t ticks);
            std::size_t current_ticks() const;

        private:
            struct Particle
            {
                Vector2<double> position;
                double radius;
                sf::Color color;
                std::size_t deletion_time;                
            };

            std::deque<Particle> particles_;
            std::size_t current_ticks_ = 0;

            std::vector<sf::Vertex> vertex_buffer_;
        };
    }
}

#endif