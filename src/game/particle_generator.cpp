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

#include "particle_generator.hpp"

#include "world/car.hpp"
#include "core/transform.hpp"

#include "resources/terrain_definition.hpp"

#include <algorithm>

void ts::game::Particle_generator::on_car_create(world::Car* car)
{
    car_list_.emplace_back();
    car_list_.back().car = car;
}

void ts::game::Particle_generator::on_entity_destroy(world::Entity* entity)
{
    auto pred = [entity](const Car_info& car_info)
    {
        return car_info.car == entity;
    };

    car_list_.erase(std::remove_if(car_list_.begin(), car_list_.end(), pred), car_list_.end());
}

void ts::game::Particle_generator::update(std::size_t ticks)
{
    Particle_drawer::update(ticks);

    std::uniform_real_distribution<double> offset_dist(-3.0, 3.0);
    std::uniform_real_distribution<double> size_dist(0.5, 1.5);

    for (auto& car_info : car_list_)
    {
        auto car = car_info.car;
        auto current_traction = car->current_traction();
        const auto& terrain = car->current_terrain();
        const auto& car_definition = car->car_definition();

        auto slide_threshold = car_definition.handling.slide_threshold;

        auto time_passed = current_ticks() - car_info.last_ticks;

        if (car->speed() > 0.0 && (terrain.roughness > 0.0 ? time_passed >= std::size_t(50.0 / terrain.roughness) :
            terrain.tire_mark && current_traction < slide_threshold && time_passed >= 30U))
        {
            const auto& tire_positions = car_definition.tire_positions;

            if (car_info.current_tire >= tire_positions.size()) car_info.current_tire = 0;

            if (car_info.current_tire < tire_positions.size())
            {
                auto tire_position = car_definition.tire_positions[car_info.current_tire];
                tire_position = world::transform_point(tire_position, car->rotation());

                sf::Color color(200, 200, 200, 150);
                

                if (terrain.roughness > 0.0)
                {
                    const auto& terrain_color = terrain.color;
                    color = sf::Color(terrain_color.red * 0.67, terrain_color.green * 0.67, terrain_color.blue * 0.67);
                }


                auto offset = offset_dist(random_engine_);
                auto size = size_dist(random_engine_);
                create_particle(car->position() + tire_position + offset, size, color, 200U);

                ++car_info.current_tire;
                car_info.last_ticks = current_ticks();
            }
        }
    }
}