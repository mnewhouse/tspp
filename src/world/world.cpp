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
#include "world.hpp"
#include "car.hpp"
#include "collisions.hpp"

#include "resources/track.hpp"
#include "resources/pattern.hpp"

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


ts::world::World::World(resources::Track track, resources::Pattern track_pattern)
: track_(std::move(track)), 
  terrain_map_(std::move(track_pattern)),
  scenery_bitmap_(terrain_map_, track_.terrain_library(), track_.num_levels()),
  control_point_manager_(track_.control_points().begin(), track_.control_points().end())
{
    add_world_listener(&control_point_manager_);
}

ts::world::Car* ts::world::World::create_car(const resources::Car_definition& car_def, std::int32_t start_position)
{
    const auto& start_points = track().start_points();
    if (start_position >= start_points.size())
    {
        return nullptr;
    }

    car_list_.push_back(std::make_unique<Car>(this, car_def));

    auto car = &*car_list_.back();
    register_entity(car);

    const auto& start_point = start_points[start_position];
    car->set_position(start_point.position);
    car->set_rotation(start_point.rotation);
    car->set_z_position(start_point.level);

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

const ts::resources::Terrain_definition& ts::world::World::terrain_at(Vector2i point) const
{
    auto x = static_cast<std::size_t>(point.x);
    auto y = static_cast<std::size_t>(point.y);

    auto terrain_id = terrain_map_(x, y);

    return track_.terrain_library().terrain_by_id(terrain_id);
}

const ts::resources::Terrain_definition& ts::world::World::terrain_at(Vector2i point, std::size_t level) const
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
    double fd = frame_duration * 0.001;

    auto entity_count = entity_list_.size();

    state_buffer_.resize(entity_count);
    position_buffer_.resize(entity_count);

    collision_buffer_.clear();

    for (std::size_t index = 0; index != entity_count; ++index)
    {
        auto entity = entity_list_[index];
        auto& new_state = state_buffer_[index];
        auto& old_position = position_buffer_[index];

        entity->update(fd);

        new_state.entity = entity;
        new_state.position = clamp_position(compute_new_position(*entity, fd), track_.size());
        new_state.rotation = compute_new_rotation(*entity, fd);
        new_state.velocity = entity->velocity();
        new_state.angular_velocity = entity->angular_velocity();
        new_state.z_position = entity->z_position();
      
        old_position.position = entity->position();
        old_position.z_level = entity->z_level();
        old_position.entity = entity;    
    }

    auto collision_priority_test = [](const Collision_result& a, const Collision_result& b)
    {
        if (a && b) return a.valid_time_point < b.valid_time_point;

        return a && !b;
    };
    
    auto insert_collision = [=](const Collision_result& collision)
    {
        const auto subject = collision.subject_state.entity;
        const auto object = collision.object_state.entity;

        auto insert_position = std::lower_bound(collision_buffer_.begin(), collision_buffer_.end(), collision, collision_priority_test);

        auto conflict_pred = [subject, object](const Collision_result& collision)
        {
            return Match_collision(subject)(collision) || (object && Match_collision(object)(collision));
        };

        auto conflict_it = std::find_if(collision_buffer_.begin(), insert_position, conflict_pred);
        if (conflict_it == insert_position)
        {
            // No conflicts. Proceed with the insertion.
            insert_position = collision_buffer_.insert(insert_position, collision);
            
            // Erase conflicting collisions after insertion position
            collision_buffer_.erase(std::remove_if(std::next(insert_position), collision_buffer_.end(), conflict_pred), collision_buffer_.end());
        }
    };

    for (auto it = state_buffer_.begin(); it != state_buffer_.end(); ++it)
    {
        auto entity = it->entity;

        auto entity_collision = detect_entity_collision(*it, std::next(it), state_buffer_.end(), collision_priority_test);
        auto scenery_collision = detect_scenery_collision(*it, *this, scenery_bitmap_);

        const auto& collision = std::min(entity_collision, scenery_collision, collision_priority_test);
        if (collision)
        {
            insert_collision(collision);
        }
    }

    auto get_new_collision = [=](const Entity_state& entity_state, double time_point)
    {
        auto scenery_collision = detect_scenery_collision(entity_state, *this, scenery_bitmap_);

        auto entity_collision = detect_entity_collision(entity_state, state_buffer_.begin(), state_buffer_.end(), 
                                                        collision_priority_test);        

        auto collision = std::min(entity_collision, scenery_collision, collision_priority_test);        
        if (collision)
        {
            auto time_left = 1.0 - time_point;
            collision.time_point = time_point + time_left * collision.time_point;
            collision.valid_time_point = time_point + time_left * std::max(collision.valid_time_point, 0.0);
        }

        return collision;
    };

    double last_time_point = 0.0;
    while (!collision_buffer_.empty())
    {
        auto collision = collision_buffer_.front();
        collision_buffer_.pop_front();

        double time_point = std::max(collision.valid_time_point, 0.0);
        double time_left = 1.0 - time_point;
        double time_step = time_point - last_time_point;
        
        const auto subject = collision.subject_state.entity;
        const auto object = collision.object_state.entity;

        Entity_state* subject_state = nullptr;
        Entity_state* object_state = nullptr;

        for (auto& target_state : state_buffer_)
        {
            auto entity = target_state.entity;

            if (entity == subject || entity == object)
            {                
                const auto& resolved_state = (subject == entity ? collision.subject_state : collision.object_state);
                (subject == entity ? subject_state : object_state) = &target_state;

                target_state = resolved_state;                  

                if (magnitude(target_state.velocity) > max_entity_speed)
                {
                    target_state.velocity = normalize(target_state.velocity) * max_entity_speed;
                }

                if (!collision.rotate)
                {
                    target_state.rotation = entity->rotation();
                }

                apply_entity_state(target_state);

                target_state.position = clamp_position(compute_new_position(*entity, time_left * fd), track_.size());     
            }

            else if (time_step > 0.0)
            {
                auto time_frame = 1.0 - last_time_point;
                if (time_frame > 0.0)
                {
                    auto position = interpolate_position(entity->position(), target_state.position, time_step / time_frame);

                    displace_entity(entity, position);

                    entity->set_rotation(target_state.rotation);
                }
            }
        }

        if (!collision.rotate)
        {
            if (!object)
            {
                // Try to rotate the entity by displacing it
                auto old_state = *subject_state;

                *subject_state = displace_unrotated_entity(collision, scenery_bitmap_);
                apply_entity_state(*subject_state);

                auto new_collision = detect_entity_collision(*subject_state, state_buffer_.begin(), state_buffer_.end(), collision_priority_test);
                if (new_collision)
                {
                    // Displacement cannot happen, revert
                    *subject_state = old_state;
                    apply_entity_state(old_state);
                }
            }
        }

        if (!collision.stuck)
        {
            // Get new collision
            if (subject_state)
            {
                auto subject_collision = get_new_collision(*subject_state, time_point);
                if (subject_collision)
                {
                    insert_collision(subject_collision);
                }
            }

            if (object_state)
            {
                auto object_collision = get_new_collision(*object_state, time_point);
                if (object_collision)
                {
                    insert_collision(object_collision);
                }
            }
        }

        for (auto listener : world_listeners_)
        {
            listener->on_collision(collision);
        }

        last_time_point = time_point;
    }

    // Finally, set the entities' new states
    for (auto& entity_state : state_buffer_) 
    {
        entity_state.position = clamp_position(entity_state.position, track_.size());

        apply_entity_state(entity_state);
    }

    for (auto& entity_position : position_buffer_)
    {
        auto entity = entity_position.entity;
        auto old_position = entity_position.position;

        control_point_manager_.test_control_point_intersection(entity, entity->position(), old_position);
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

void ts::world::World::apply_entity_state(const Entity_state& new_state)
{
    auto entity = new_state.entity;

    displace_entity(entity, new_state.position);

    entity->set_velocity(new_state.velocity);
    entity->set_angular_velocity(new_state.angular_velocity);

    entity->set_rotation(new_state.rotation);
}

void ts::world::World::terrain_transition(Entity* entity, const resources::Terrain_definition& old_terrain,
                                          const resources::Terrain_definition& new_terrain)
{
    const auto& terrain_library = track_.terrain_library();

    auto old_level = entity->z_level();
    auto new_level = old_level;

    auto terrain_above = terrain_library.sub_terrain(new_terrain.id, old_level + 1);
    if (terrain_above != 0 && !terrain_library.terrain_by_id(terrain_above).is_wall)
    {
        new_level = old_level + 1;
    }

    else if (old_level != 0)
    {
        auto terrain_below = terrain_library.sub_terrain(new_terrain.id, old_level - 1);

        if (terrain_below != 0 && !terrain_library.terrain_by_id(terrain_below).is_wall)
        {
            new_level = old_level - 1;
        }
    }

    if (new_level != old_level)
    {
        auto old_z_position = entity->z_position();
        entity->set_z_position(new_level);

        if (entity_collides(entity))
        {
            // Cannot put the entity on the new level
            entity->set_z_position(old_z_position);
        }
    }
}

bool ts::world::World::entity_collides(Entity* entity)
{
    const auto& collision_bitmap = entity->collision_bitmap();
    const auto z_level = entity->z_level();
    const auto position = entity->position();
    const auto rotation = entity->rotation();

    if (collision_test(collision_bitmap, scenery_bitmap_, position, rotation, z_level))
    {
        return true;
    }

    for (auto other : entity_list_)
    {
        if (other == entity || other->z_level() != z_level) continue;                   
                
        if (collision_test(collision_bitmap, other->collision_bitmap(), 
            position, other->position(), rotation, other->rotation()))
        {
            return true;
        }
    }

    return false;
}

void ts::world::World::displace_entity(Entity* entity, Vector2<double> new_position)
{
    bresenham(entity->position(), new_position, [this, entity](Vector2i position)
    {
        const auto& old_terrain = terrain_at(entity->position());
        const auto& new_terrain = terrain_at(position);

        if (old_terrain != new_terrain)
        {
            terrain_transition(entity, old_terrain, new_terrain);
        }

        entity->set_position(position);
    });

    entity->set_position(new_position);
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

void ts::world::World::remove_world_listener(World_listener* world_listener)
{
    world_listeners_.erase(std::remove(world_listeners_.begin(), world_listeners_.end(), world_listener), world_listeners_.end());
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