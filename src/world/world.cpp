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
        }

        if (!collision.stuck && collision.collided) {
            collision_queue_.push_back(collision);
        }
    }

    auto collision_compare = [](const Collision_result& a, const Collision_result& b)
    {
        return a.time_point < b.time_point;
    };

    auto resolve_collision = [this](const Collision_result& collision)
    {
        auto entity = collision.subject;
        auto normal = collision.normal;

        auto velocity = entity->velocity();
        auto dot = velocity.x * collision.normal.x + velocity.y * collision.normal.y;

        Vector2d new_velocity = { -2.0 * dot * normal.x + velocity.x, -2.0 * dot * normal.y + velocity.y };
        new_velocity *= 0.5;

        entity->set_position(clamp_position(collision.subject_position));
        entity->set_rotation(collision.subject_rotation);
        entity->set_velocity(new_velocity);
    };

    auto resolve_inter_entity_collision = [this](const Collision_result& collision)
    {
        auto relative_velocity = collision.subject->velocity() - collision.object->velocity();
        
        collision.object->set_velocity(collision.subject->velocity());
        collision.subject->set_velocity({});
        
        collision.subject->set_rotation(collision.subject_rotation);        
        collision.object->set_rotation(collision.object_rotation);

        collision.subject->set_position(clamp_position(collision.subject_position));
        collision.object->set_position(clamp_position(collision.object_position));
    };

    auto get_new_collision = [this, detect_entity_collision](const Entity_state& state)
    {
        Collision_result collision = detect_collision(state, terrain_map_, track_.terrain_library());

        for (const auto& other_state : state_buffer_) {
            if (other_state.entity == state.entity) continue;

            detect_entity_collision(collision, state, other_state);
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

        // Check if there are any conflicting collions prior to the insertion position
        auto search_result = std::find_if(collision_queue_.begin() + 1, insert_position, pred);
        if (search_result == insert_position) {
            // Perform the insertion. God that sounds dirty
            insert_position = collision_queue_.insert(insert_position, new_collision);

            // Finally, erase all conflicting collisions after the one we just inserted
            collision_queue_.erase(std::remove_if(insert_position + 1, collision_queue_.end(), pred), collision_queue_.end());
        }
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

    int collision_count = 0;

    while (!collision_queue_.empty())
    {
        const auto& collision = collision_queue_.front();

        if (collision.subject && collision.object) {
            resolve_inter_entity_collision(collision);
        }

        else if (collision.subject) {
            resolve_collision(collision);
        }

        const Target_state* subject_state = nullptr;
        const Target_state* object_state = nullptr;

        auto time_left = 1.0 - collision.time_point;

        // Update the position for all entities
        for (auto& target_state : state_buffer_) {
            auto entity = target_state.entity;

            if (entity == collision.subject || entity == collision.object) {
                target_state.position = entity->position() + (entity->velocity() * fd * time_left);

                if (collision.rotate) {
                    auto delta_r = entity->rotation().radians() * fd * time_left * entity->angular_velocity();
                    target_state.rotation = entity->rotation() + Rotation<double>::radians(delta_r);
                }

                else {
                    target_state.rotation = entity->rotation();
                }

                target_state.time_point = 1.0;

                if (entity == collision.subject) subject_state = &target_state;
                else if (entity == collision.object) object_state = &target_state;
            }

            else {
                auto duration = target_state.time_point - last_time_point;
                move_towards_position(entity, target_state.position, duration);
                move_towards_rotation(entity, target_state.rotation, duration);
            }
        }

        if (subject_state && !collision.stuck)
        {
            auto new_collision = get_new_collision(*subject_state);
            if (new_collision.collided) {
                new_collision.time_point = collision.time_point + (time_left * new_collision.time_point);
                add_new_collision(new_collision);
            }
        }

        if (object_state && !collision.stuck) 
        {
            auto new_collision = get_new_collision(*object_state);
            if (new_collision.collided) {
                new_collision.time_point = collision.time_point + (time_left * new_collision.time_point);
                add_new_collision(new_collision);
            }
        }

        
        last_time_point = collision.time_point;

        collision_queue_.pop_front();

        ++collision_count;
    }

    // Finally, set the entities' new states
    for (auto& state : state_buffer_) {
        state.entity->set_position(state.position);
        state.entity->set_rotation(state.rotation);
    }
}
