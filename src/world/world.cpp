/*
 * Turbo Sliders++
 * Copyright (C) 2013 Martin Newhouse
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

#include "world.hpp"
#include "car.hpp"
#include "entity_listener.hpp"

#include "resources/track.hpp"
#include "resources/pattern.hpp"

#include "collisions.hpp"

#include <algorithm>

ts::world::World::World(resources::Track&& track)
: track_(std::move(track)), terrain_map_(track_.size())
{
    resources::build_track_pattern(track_, terrain_map_);
}


ts::world::Car* ts::world::World::create_car(const resources::Car_definition& car_def)
{
    car_list_.push_back(std::make_unique<Car>(this, car_def));

    auto& car = *car_list_.back();
    std::for_each(entity_listeners_.begin(), entity_listeners_.end(), 
        [&](Entity_listener* listener)
        {
            listener->on_car_create(car);
        }
    );

    car.set_position({ 200.0, 200.0 });

    return &car;
}

void ts::world::World::add_entity_listener(Entity_listener* entity_listener)
{
    entity_listeners_.push_back(entity_listener);
}

const ts::resources::Terrain_definition& ts::world::World::terrain_at(Vector2d point) const
{
    auto x = std::size_t(point.x + 0.5);
    auto y = std::size_t(point.y + 0.5);

    auto terrain_id = terrain_map_(x, y);

    return track_.terrain_library().terrain_by_id(terrain_id);
}

const ts::resources::Terrain_definition& ts::world::World::terrain_at(Vector2d point, std::size_t level) const
{
    auto terrain = terrain_at(point);

    const auto& terrain_lib = track_.terrain_library();
    auto sub_terrain = terrain_lib.sub_terrain(terrain.id, level);
    return terrain_lib.terrain_by_id(sub_terrain);
}

void ts::world::World::update(std::size_t frame_duration)
{
    double fd = frame_duration / 1000.0;

    // Update all entities
    for (const auto& car : car_list_) {
        car->update(fd);

        auto old_position = car->position();
        auto old_rotation = car->rotation();

        // Get the new position that we're trying to move to
        auto new_position = old_position + car->velocity() * fd;

        // Clamp to valid track region
        new_position.x = std::max(new_position.x, 0.0);
        new_position.x = std::min<double>(new_position.x, track_.size().x);

        new_position.y = std::max(new_position.y, 0.0);
        new_position.y = std::min<double>(new_position.y, track_.size().y);
        
        auto velocity = car->velocity(), new_velocity = velocity;
        auto new_rotation = old_rotation + Rotation<double>::radians(car->angular_velocity() * fd);
        
        auto collision_result = detect_collision(*car, new_position, new_rotation, terrain_map_, track_.terrain_library());
        auto time_point = 0.0;

        while (collision_result.collided && !collision_result.stuck) {
            auto normal = normalize(collision_result.normal);
            auto time_elapsed = ((1.0 - time_point) * collision_result.time_point);
            auto dot = velocity.x * normal.x + velocity.y * normal.y;

            new_velocity = { -2.0 * dot * normal.x + velocity.x, -2.0 * dot * normal.y + velocity.y };
            new_velocity *= 0.5;

            new_position = collision_result.position + (new_velocity * fd * time_elapsed);

            car->set_position(collision_result.position);
            car->set_rotation(new_rotation);
            car->set_velocity(new_velocity);

            time_point += time_elapsed;

            collision_result = detect_collision(*car, new_position, new_rotation, terrain_map_, track_.terrain_library());
        }

        if (!collision_result.stuck) {
            // New position found.
            car->set_position(new_position);
            car->set_rotation(new_rotation);
            car->set_velocity(new_velocity);

            // Now update the z position, if necessary.            
            auto level_transition = [this, &new_position, &new_rotation, &car](int level, int offset)
            {
                car->set_z_position(double(level + offset));

                // Test if there's room for the car at its new level
                auto collision_result = detect_collision(*car, new_position, new_rotation, terrain_map_, track_.terrain_library());
                if (collision_result.collided) {
                    // If there's not, restore the z position to its old value.
                    car->set_z_position(double(level));
                }

                return !collision_result.collided;
            };

            auto level = int(car->z_position());

            // While there's a non-null terrain above us, attempt to climb one level.
            while (terrain_at(new_position, level + 1).id != 0 && level_transition(level, +1)) {
                ++level;
            }

            if (terrain_at(new_position, level).id == 0) {
                while (level > 0 && terrain_at(new_position, level - 1).id != 0 && level_transition(level, -1)) {
                    --level;
                }
            }
        }

        else {
            // We got stuck somehow :(. Restore the old position.
            car->set_position(old_position);
            car->set_rotation(old_rotation);
        }
    }
}
