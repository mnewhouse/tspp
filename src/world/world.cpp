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

#include "core/utility.hpp"

#include "collisions.hpp"

#include <algorithm>
#include <deque>
#include <iostream>

ts::world::World::World(resources::Track&& track)
: track_(std::move(track)), terrain_map_(track_.size())
{
    resources::build_track_pattern(track_, terrain_map_);
}


ts::world::Car* ts::world::World::create_car(const resources::Car_definition& car_def)
{
    car_list_.push_back(make_unique<Car>(this, car_def));

    auto& car = *car_list_.back();
    std::for_each(entity_listeners_.begin(), entity_listeners_.end(), 
        [&](Entity_listener* listener)
        {
            listener->on_car_create(car);
        }
    );

    car.set_position({ 200.0, 200.0 });

    entity_list_.push_back(&car);

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

ts::Vector2d ts::world::World::clamp_position(Vector2d position) const
{
    position.x = std::max(position.x, 0.0);
    position.x = std::min<double>(position.x, track_.size().x);

    position.y = std::max(position.y, 0.0);
    position.y = std::min<double>(position.y, track_.size().y);

    return position;
}

void ts::world::World::update(std::size_t frame_duration)
{
    double fd = frame_duration / 1000.0;

    state_buffer_.resize(entity_list_.size());

    for (auto entity : entity_list_) {
        entity->update(fd);
    }

    std::transform(entity_list_.begin(), entity_list_.end(), state_buffer_.begin(), 
        [this, fd](Entity* entity)
        {
            Target_state state;
            state.entity = entity;
            state.position = clamp_position(entity->position() + entity->velocity() * fd);
            state.rotation = entity->rotation() + Rotation<double>::radians(entity->angular_velocity() * fd);

            return state;
        }
    );

    auto detect_entity_collision = [](Collision_result& collision, const Entity_state& state_a, const Entity_state& state_b)
    {
        auto entity_collision = detect_collision(state_a, state_b);
        if (entity_collision.collided && (!collision.collided || entity_collision.time_point < collision.time_point)) {
            collision = entity_collision;
        }
    };

    collision_queue_.clear();

    for (auto it = state_buffer_.begin(); it != state_buffer_.end(); ++it)
    {
        Collision_result collision = detect_collision(*it, terrain_map_, track_.terrain_library());

        for (auto other_it = std::next(it); other_it != state_buffer_.end(); ++other_it)
        {
            detect_entity_collision(collision, *it, *other_it);
            detect_entity_collision(collision, *other_it, *it);
        }

        if (!collision.stuck && collision.collided) {
            collision_queue_.push_back(collision);
        }
    }

    auto collision_compare = [](const Collision_result& a, const Collision_result& b)
    {
        return a.time_point < b.time_point;
    };

    auto resolve_collision = [](Entity* entity, const Vector2<double>& position, Rotation<double> rotation, const Vector2<double>& normal)
    {
        auto velocity = entity->velocity();
        auto dot = velocity.x * normal.x + velocity.y * normal.y;

        Vector2d new_velocity = { -2.0 * dot * normal.x + velocity.x, -2.0 * dot * normal.y + velocity.y };
        new_velocity *= 0.5;

        entity->set_position(position);
        entity->set_rotation(rotation);
        entity->set_velocity(new_velocity);
    };

    auto get_new_collision = [this, detect_entity_collision](const Entity_state& state)
    {
        Collision_result collision = detect_collision(state, terrain_map_, track_.terrain_library());

        for (const auto& other_state : state_buffer_) {
            if (other_state.entity == state.entity) continue;

            detect_entity_collision(collision, state, other_state);
            detect_entity_collision(collision, other_state, state);
        }

        return collision;
    };

    auto add_new_collision = [this, collision_compare](const Collision_result& new_collision)
    {
        auto pred = [&new_collision](const Collision_result& result)
        {
            return (new_collision.subject == result.subject || new_collision.subject == result.object) ||
                (new_collision.object != nullptr && (new_collision.object == result.subject || new_collision.object == result.object));
        };

        // Get the insert position
        auto insert_position = std::lower_bound(collision_queue_.begin() + 1, collision_queue_.end(), new_collision, collision_compare);

        // Perform the insertion
        insert_position = collision_queue_.insert(insert_position, new_collision);

        // Finally, erase all conflicting collisions after the one we just inserted
        collision_queue_.erase(std::remove_if(insert_position + 1, collision_queue_.end(), pred), collision_queue_.end());
    };

    auto move_towards_position = [fd](Entity* entity, const Vector2<double>& position, double duration)
    {
        auto old_position = entity->position();
        auto new_position = old_position + ((position - old_position) * fd * duration);
        entity->set_position(new_position);
    };

    auto move_towards_rotation = [fd](Entity* entity, Rotation<double> rotation, double duration)
    {
        auto old_rotation = entity->rotation();

        auto diff = rotation - old_rotation;
        auto new_rotation = old_rotation + Rotation<double>::radians(diff.radians() * fd * duration);
        entity->set_rotation(new_rotation);
    };


    std::sort(collision_queue_.begin(), collision_queue_.end(), collision_compare);

    double last_time_point = 0.0;

    while (!collision_queue_.empty())
    {
        const auto& collision = collision_queue_.front();

        auto normal = normalize(collision.normal);

        if (collision.subject) {
            resolve_collision(collision.subject, collision.subject_position, collision.subject_rotation, normal);
        }

        if (collision.object) {
            resolve_collision(collision.object, collision.object_position, collision.object_rotation, normal);
        }

        // Update the position for all entities
        for (auto& target_state : state_buffer_) {
            auto entity = target_state.entity;

            if (entity == collision.subject || entity == collision.object) {
                auto time_left = 1.0 - collision.time_point;

                target_state.position = entity->position() + (entity->velocity() * fd * time_left);
                target_state.rotation = entity->rotation() + Rotation<double>::radians(entity->rotation().radians() * fd * time_left);
                target_state.time_point = 1.0;

                auto new_collision = get_new_collision(target_state);

                if (new_collision.stuck) {
                    // Car in wall, this is ass!

                }

                else if (new_collision.collided) {
                    new_collision.time_point = collision.time_point + (time_left * new_collision.time_point);
                    add_new_collision(new_collision);

                    entity->set_velocity({0, 0});
                }
            }

            else {
                auto duration = target_state.time_point - last_time_point;
                move_towards_position(entity, target_state.position, duration);
                move_towards_rotation(entity, target_state.rotation, duration);
            }
        }

        
        last_time_point = collision.time_point;

        collision_queue_.pop_front();
    }

    for (auto& state : state_buffer_) {
        auto entity = state.entity;

        entity->set_position(state.position);
        entity->set_rotation(state.rotation);
    }

    /*
    // Update all entities
    for (const auto& car : car_list_) {
        car->update(fd);

        auto old_position = car->position();
        auto old_rotation = car->rotation();

        Entity_state car_state;
        car_state.entity = &*car;

        auto& new_position = car_state.new_position;
        auto& new_rotation = car_state.new_rotation;

        // Get the new position that we're trying to move to
        new_position = old_position + car->velocity() * fd;

        // Clamp to valid track region
        new_position.x = std::max(new_position.x, 0.0);
        new_position.x = std::min<double>(new_position.x, track_.size().x);

        new_position.y = std::max(new_position.y, 0.0);
        new_position.y = std::min<double>(new_position.y, track_.size().y);
        
        auto velocity = car->velocity(), new_velocity = velocity;

        new_rotation = old_rotation + Rotation<double>::radians(car->angular_velocity() * fd);
        
        auto collision_result = detect_collision(car_state, terrain_map_, track_.terrain_library());
        auto time_point = 0.0;

        while (collision_result.collided && !collision_result.stuck) {
            auto normal = normalize(collision_result.normal);
            auto time_elapsed = ((1.0 - time_point) * collision_result.time_point);
            auto dot = velocity.x * normal.x + velocity.y * normal.y;

            new_velocity = { -2.0 * dot * normal.x + velocity.x, -2.0 * dot * normal.y + velocity.y };
            new_velocity *= 0.5;

            new_position = collision_result.subject_position + (new_velocity * fd * time_elapsed);

            car->set_position(collision_result.subject_position);
            car->set_rotation(new_rotation);
            car->set_velocity(new_velocity);

            time_point += time_elapsed;

            collision_result = detect_collision(car_state, terrain_map_, track_.terrain_library());
        }

        if (!collision_result.stuck) {
            // New position found.
            car->set_position(new_position);
            car->set_rotation(new_rotation);
            car->set_velocity(new_velocity);

            // Now update the z position, if necessary.            
            auto level_transition = [this, &car_state, &car](int level, int offset)
            {
                car->set_z_position(double(level + offset));

                // Test if there's room for the car at its new level
                auto collision_result = detect_collision(car_state, terrain_map_, track_.terrain_library());
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
    */
}
