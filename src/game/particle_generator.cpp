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
#include "particle_generator.hpp"

#include "world/car.hpp"

#include "resources/terrain_definition.hpp"

void ts::game::Particle_generator::add_car(const world::Car* car)
{
    car_list_.emplace_back();
    car_list_.back().car = car;
}

void ts::game::Particle_generator::remove_car(const world::Car* car)
{
    auto car_test = [car](const Car_info& car_info)
    {
        return car_info.car == car;
    };

    car_list_.erase(std::remove_if(car_list_.begin(), car_list_.end(), car_test), car_list_.end());
}

void ts::game::Particle_generator::render(sf::RenderTarget& render_target, sf::RenderStates render_states)
{
    particle_drawer_.render(render_target, render_states);
}

void ts::game::Particle_generator::update(std::size_t ticks)
{
    particle_drawer_.update(ticks);

    std::uniform_real_distribution<double> offset_dist(-3.0, 3.0);
    std::uniform_real_distribution<double> size_dist(0.5, 1.5);

    for (auto& car_info : car_list_)
    {
        auto car = car_info.car;
        auto current_traction = car->current_traction();
        const auto& terrain = car->current_terrain();
        const auto& car_definition = car->car_definition();

        auto slide_threshold = car_definition.handling.slide_threshold;

        auto time_passed = particle_drawer_.current_ticks() - car_info.last_ticks;

        if (car->speed() > 0.0 && (terrain.roughness > 0.0 ? time_passed >= std::size_t(50.0 / terrain.roughness) :
            terrain.tire_mark && current_traction < slide_threshold && time_passed >= 30U))
        {
            const auto& tire_positions = car_definition.tire_positions;

            if (car_info.current_tire >= tire_positions.size()) car_info.current_tire = 0;

            if (car_info.current_tire < tire_positions.size())
            {
                auto tire_position = car_definition.tire_positions[car_info.current_tire];
                tire_position = transform_point(tire_position, car->rotation());

                sf::Color color(200, 200, 200, 150);

                if (terrain.roughness > 0.0)
                {
                    const auto& terrain_color = terrain.color;
                    const double multiplier = 0.67;

                    auto red = static_cast<sf::Uint8>(terrain_color.red * multiplier);
                    auto green = static_cast<sf::Uint8>(terrain_color.green * multiplier);
                    auto blue = static_cast<sf::Uint8>(terrain_color.blue * multiplier);

                    color = sf::Color(red, green, blue);
                }

                if (terrain.skid_mark || terrain.roughness > 0.0)
                {
                    auto offset = offset_dist(random_engine_);
                    auto size = size_dist(random_engine_);
                    particle_drawer_.create_particle(car->position() + tire_position + offset, size, color, 200U);

                    ++car_info.current_tire;
                    car_info.last_ticks = particle_drawer_.current_ticks();
                }
            }
        }
    }
}