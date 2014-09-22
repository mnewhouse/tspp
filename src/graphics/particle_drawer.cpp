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

#include "stdinc.hpp"
#include "particle_drawer.hpp"

void ts::graphics::Particle_drawer::create_particle(Vector2<double> position, double radius, sf::Color color, std::size_t duration)
{
    Particle particle;
    particle.position = position;
    particle.radius = radius;
    particle.color = color;
    particle.deletion_time = current_ticks_ + duration;

    auto compare = [](const Particle& a, const Particle& b)
    {
        return a.deletion_time < b.deletion_time;
    };

    auto insertion_point = std::lower_bound(particles_.begin(), particles_.end(), particle, compare);
    particles_.insert(insertion_point, particle);
}

void ts::graphics::Particle_drawer::update(std::size_t ticks)
{
    current_ticks_ += ticks;

    auto deletion_end = std::lower_bound(particles_.begin(), particles_.end(), current_ticks_, 
        [](const Particle& particle, std::size_t ticks)
    {
        return particle.deletion_time < ticks;
    });

    particles_.erase(particles_.begin(), deletion_end);
}

std::size_t ts::graphics::Particle_drawer::current_ticks() const
{
    return current_ticks_;
}

void ts::graphics::Particle_drawer::render(sf::RenderTarget& render_target, sf::RenderStates render_states)
{
    std::array<Vector2<double>, 12> points
    {
        {
            { 0.0, -1.0 },
            { 0.707107, 0.707107 },
            { 1.0, 0.0 },
            { 0.707107, -0.707107 },

            { 0.0, -1.0 },
            { -0.707107, 0.707107 },
            { 0.0, 1.0 },
            { 0.707107, 0.707107 },

            { 0.0, -1.0 },
            { -0.707107, -0.707107 },
            { -1.0, 0.0 },
            { -0.707107, 0.707107 }
        }
    };

    vertex_buffer_.resize(particles_.size() * points.size());

    auto buffer_it = vertex_buffer_.begin();

    for (const auto& particle : particles_)
    {
        auto make_vertex = [&particle](Vector2<double> point)
        {
            point *= particle.radius;
            point += particle.position;

            sf::Vertex vertex;
            vertex.position.x = static_cast<float>(point.x);
            vertex.position.y = static_cast<float>(point.y);
            vertex.color = particle.color;
            return vertex;            
        };

        buffer_it = std::transform(points.begin(), points.end(), buffer_it, make_vertex);
    }

    if (!vertex_buffer_.empty()) 
    {
        render_target.draw(vertex_buffer_.data(), static_cast<unsigned int>(vertex_buffer_.size()), sf::Quads, render_states);
    }
}