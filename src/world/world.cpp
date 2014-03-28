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

#include "world.hpp"
#include "car.hpp"
#include "collisions.hpp"

#include "resources/track.hpp"
#include "resources/pattern.hpp"

#include "core/utility.hpp"

#include <algorithm>
#include <deque>
#include <fstream>

namespace ts
{
    namespace world
    {
        inline Vector2<double> compute_new_position(const Entity& entity, double frame_duration)
        {
            return entity.position() + entity.velocity() * frame_duration;
        }

        inline Rotation<double> compute_new_rotation(const Entity& entity, double frame_duration)
        {
            return entity.rotation() + Rotation<double>::radians(entity.angular_velocity() * frame_duration);
        }

        inline Vector2<double> clamp_position(Vector2<double> position, const Vector2<double>& world_size);
    }
}


ts::world::World::World(resources::Track&& track)
: track_(std::move(track)), 
  terrain_map_(resources::build_track_pattern(track_)),
  scenery_bitmap_(terrain_map_, track_.terrain_library(), track_.num_levels()),
  control_point_manager_(track_.control_points().begin(), track_.control_points().end())
{
    add_world_listener(&control_point_manager_);
}

ts::world::Car* ts::world::World::create_car(const resources::Car_definition& car_def)
{
    car_list_.push_back(std::make_unique<Car>(this, car_def));

    auto car = &*car_list_.back();
    register_entity(car);
    
    for (auto listener : world_listeners_)
    {
        listener->on_car_create(car);
    }    

    return car;
}

void ts::world::World::launch_game()
{
    started_ = true;
    for (auto listener : world_listeners_)
    {
        listener->on_start();
    }
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

ts::Vector2<double> ts::world::clamp_position(Vector2<double> position, const Vector2<double>& world_size)
{
    position.x = std::max(position.x, 0.0);
    position.x = std::min<double>(position.x, std::nexttoward(world_size.x, 0.0));

    position.y = std::max(position.y, 0.0);
    position.y = std::min<double>(position.y, std::nexttoward(world_size.y, 0.0));

    return position;
}

void ts::world::World::update(std::size_t frame_duration)
{
    double fd = frame_duration / 1000.0;

    auto entity_count = entity_set_.size();
    state_buffer_.resize(entity_count);

    for (auto entity : entity_set_) {
        entity->update(fd);
    }

    state_buffer_.resize(entity_count);
    position_buffer_.resize(entity_count);

    std::transform(entity_list_.begin(), entity_list_.end(), position_buffer_.begin(), 
                   [](Entity* entity)
    {
        return std::make_pair(entity, entity->position());
    });

    std::transform(entity_list_.begin(), entity_list_.end(), state_buffer_.begin(), [this, fd](Entity* entity)
    {
        Entity_state result;
        result.entity = entity;
        result.position = clamp_position(compute_new_position(*entity, fd), track_.size());
        result.rotation = compute_new_rotation(*entity, fd);

        return result;
    });

    auto collision_less = [](const Collision_result& a, const Collision_result& b)
    {
        if (a.collided && b.collided) return a.time_point < b.time_point;

        return a.collided;
    };
    
    auto insert_collision = [this, collision_less](const Collision_result& collision)
    {
        auto conflict_predicate = [&collision](const Collision_result& other)
        {
            return collision.subject_state.entity == other.subject_state.entity ||
                collision.subject_state.entity == other.object_state.entity ||
                collision.object_state.entity == other.subject_state.entity ||
                (collision.object_state.entity && collision.object_state.entity == other.object_state.entity);
        };

        // Find insert position
        auto insert_position = std::lower_bound(collision_queue_.begin(), collision_queue_.end(), collision, collision_less);

        // Check for conflicting collisions prior to the insertion position
        auto conflict_it = std::find_if(collision_queue_.begin(), insert_position, conflict_predicate);
        if (conflict_it == insert_position)
        {
            // Free to perform the insert
            insert_position = collision_queue_.insert(insert_position, collision);

            // And remove conflicting collisions after the inserted element
            collision_queue_.erase(std::remove_if(std::next(insert_position), collision_queue_.end(), conflict_predicate), collision_queue_.end());
        }
    };

    auto get_new_collision = [this, collision_less](const Entity_state& entity_state, double current_time_point)
    {
        auto collision = std::min(detect_scenery_collision(entity_state, scenery_bitmap_), 
                                  detect_entity_collision(entity_state, state_buffer_.begin(), state_buffer_.end()), 
                                  collision_less);

        if (collision) 
        {
            if (collision.time_point > 0.0)
            {
                current_time_point += collision.time_point * (1.0 - current_time_point);
            }

            collision.time_point = current_time_point;
        }

        return collision;
    };

    for (auto it = state_buffer_.begin(); it != state_buffer_.end(); ++it)
    {
        auto scenery_collision = detect_scenery_collision(*it, scenery_bitmap_);
        auto entity_collision = detect_entity_collision(*it, state_buffer_.begin(), state_buffer_.end());

        auto collision = std::min(scenery_collision, entity_collision, collision_less);

        if (collision) {
            collision.time_point = std::max(collision.time_point, 0.0);
            insert_collision(collision);
        }
    }

    double last_time_point = 0.0;

    while (!collision_queue_.empty())
    {
        auto collision = collision_queue_.front();
        collision_queue_.pop_front();

        auto subject_state = collision.subject_state;
        auto object_state = collision.object_state;

        auto subject = subject_state.entity;
        auto object = object_state.entity;

        subject->set_position(clamp_position(subject_state.position, track_.size()));
        subject->set_rotation(subject_state.rotation);

        if (object)
        {
            object->set_position(object_state.position);
            object->set_rotation(object_state.rotation);

            resolve_entity_collision(collision);
        }

        else {
            auto level = subject->z_level();
            const auto& terrain = terrain_at(collision.global_point, level);
            resolve_scenery_collision(collision, terrain.wall_definition.elasticity);
        }

        auto time_left = 1.0 - collision.time_point, real_time_left = time_left * fd;
        auto time_passed = collision.time_point - last_time_point, real_time_passed = time_passed * fd;

        // Update all entities
        for (auto& target_state : state_buffer_)
        {
            auto entity = target_state.entity;
            if (entity == subject || entity == object)
            {
                if (!collision.stuck)
                {
                    target_state.position = compute_new_position(*entity, real_time_left);
                    if (!collision.rotate) target_state.rotation = entity->rotation();
                }

                else
                {
                    target_state.position = entity->position();
                    target_state.rotation = entity->rotation();
                }

                (entity == subject ? subject_state : object_state) = target_state;
            }

            else if (real_time_passed > 0.0)
            {
                entity->set_position(clamp_position(compute_new_position(*entity, real_time_passed), track_.size()));
            }
        }

        if (!collision.stuck) 
        {
            auto new_collision = get_new_collision(subject_state, collision.time_point);
            if (new_collision)
            {
                new_collision.rotate = collision.rotate;
                insert_collision(new_collision);
            }

            if (object && (new_collision = get_new_collision(object_state, collision.time_point)))
            {
                new_collision.rotate = collision.rotate;
                insert_collision(new_collision);
            }
        }
        
        for (auto listener : world_listeners_)
        {
            listener->on_collision(collision);
        }

        last_time_point = collision.time_point;
    }

    // Finally, set the entities' new states
    for (const auto& state : state_buffer_) 
    {
        state.entity->set_position(clamp_position(state.position, track_.size()));
        state.entity->set_rotation(state.rotation);
    }

    for (const auto& entity_position : position_buffer_)
    {
        control_point_manager_.test_control_point_intersection(entity_position.first, entity_position.second);
    }

    for (auto listener : world_listeners_)
    {
        listener->on_update();
    }

    auto old_game_time = game_timer_.time();
    game_timer_.update(frame_duration);

    if (game_timer_.time() != old_game_time)
    {
        for (auto listener : world_listeners_)
        {
            listener->on_tick(game_timer_.time());
        }
    }
}

std::size_t ts::world::World::game_time() const
{
    return game_timer_.time();
}

void ts::world::World::start_game_timer()
{
    game_timer_.start();
}

ts::world::Control_point_manager& ts::world::World::control_point_manager()
{
    return control_point_manager_;
}

bool ts::world::World::is_entity(Entity* entity) const
{
    return entity_set_.find(entity) != entity_set_.end();
}

void ts::world::World::register_entity(Entity* entity)
{
    entity_set_.insert(entity);
    entity_list_.push_back(entity);
}

const std::shared_ptr<ts::world::Collision_bitmap>& ts::world::World::collision_bitmap
    (const std::shared_ptr<resources::Pattern>& pattern)
{
    return dynamic_bitmap_store_[pattern];
}


void ts::world::World::add_world_listener(World_listener* world_listener)
{
    world_listeners_.push_back(world_listener);
}

const ts::resources::Track& ts::world::World::track() const
{
    return track_;
}

const std::vector<std::unique_ptr<ts::world::Car>>& ts::world::World::car_list() const
{
    return car_list_;
}

const std::vector<ts::world::Entity*>& ts::world::World::entity_list() const
{
    return entity_list_;
}