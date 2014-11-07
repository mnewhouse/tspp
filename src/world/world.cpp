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

#include "resources/terrain_library.hpp"
#include "resources/track.hpp"
#include "resources/pattern.hpp"

namespace ts
{
    namespace world
    {
        static const double max_entity_speed_config = 10000.0;

        inline Vector2<double> compute_new_position(const Entity& entity, double frame_duration)
        {
            return entity.position() + entity.velocity() * frame_duration;
        }

        inline Rotation<double> compute_new_rotation(const Entity& entity, double frame_duration)
        {
            return entity.rotation() + Rotation<double>::radians(entity.angular_velocity() * frame_duration);
        }

        inline Vector2<double> clamp_position(Vector2<double> position, const Vector2<double>& world_size);
        inline Vector2<double> clamp_velocity(Vector2<double> velocity, double max_speed);
    }
}

ts::world::World::World(std::unique_ptr<resources::Track> track, resources::Pattern track_pattern)
: track_(std::move(track)), 
  terrain_map_(std::move(track_pattern)),
  world_size_(track_->size()),
  scenery_bitmap_(terrain_map_, track_->terrain_library(), track_->num_levels()),
  control_point_manager_(track_->control_points().begin(), track_->control_points().end())
{
    add_world_listener(&control_point_manager_);

    start_game_timer();
}

ts::world::World::~World()
{
}


ts::world::Car* ts::world::World::create_car(const resources::Car_definition& car_def)
{
    car_list_.push_back(std::make_unique<Car>(this, car_def));

    auto car = &*car_list_.back();
    register_entity(car);

    return car;
}

void ts::world::World::launch_game()
{
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

    return track_->terrain_library().terrain_by_id(terrain_id);
}

const ts::resources::Terrain_definition& ts::world::World::terrain_at(Vector2i point, std::size_t level) const
{
    auto terrain = terrain_at(point);

    const auto& terrain_lib = track_->terrain_library();
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

ts::Vector2<double> ts::world::clamp_velocity(Vector2<double> velocity, double max_speed)
{
    if (magnitude(velocity) > max_speed)
    {
        velocity = normalize(velocity) * max_speed;
    }

    return velocity;
}

void ts::world::World::update(std::size_t frame_duration)
{
    double fd = frame_duration * 0.001;

    auto entity_count = entity_list_.size();
    state_buffer_.resize(entity_count);
    for (std::size_t index = 0; index != entity_count; ++index)
    {
        auto entity = entity_list_[index];
        entity->update(fd);

        const auto& collision_bitmap = entity->collision_bitmap();

        auto& update_state = state_buffer_[index];
        update_state.entity = entity;
        update_state.collided_with = nullptr;
        update_state.collision_count = 0;
        update_state.current_position = entity->position();
        update_state.level = entity->z_level();
        update_state.rotation_index = collision_bitmap.rotation_index(entity->rotation());
    }

    for (auto& subject_state : state_buffer_)
    {
        const auto subject = subject_state.entity;

        auto target_position = compute_new_position(*subject, fd);
        auto target_rotation = compute_new_rotation(*subject, fd);

        const auto& collision_bitmap = subject->collision_bitmap();
        auto unrotated_index = subject_state.rotation_index;
        subject_state.rotation_index = collision_bitmap.rotation_index(target_rotation);

        if (entity_state_collides(subject_state))
        {
            target_rotation = subject->rotation();
            subject_state.rotation_index = unrotated_index;
        }       

        Vector2i last_point = subject->position();

        step_buffer_.clear();
        bresenham(subject_state.current_position, Vector2i(target_position),
                  [this, last_point](Vector2i point) mutable
        {
            if (point != last_point)
            {
                Entity_step step;
                step.old_position = last_point;
                step.new_position = point;
                step_buffer_.push_back(step);
            }

            last_point = point;
        });

        for (const auto& step : step_buffer_)
        {
            subject_state.current_position = step.old_position;

            auto collision_point = collision_test(collision_bitmap, scenery_bitmap_, step.new_position,
                                                  subject_state.rotation_index, subject_state.level);

            if (collision_point)
            {
                auto wall_elasticity = terrain_at(collision_point.point, subject_state.level).wall_definition.elasticity;

                auto collision_info = resolve_scenery_collision(collision_point.point, scenery_bitmap_, subject, step.new_position, wall_elasticity);
                handle_collision(subject, nullptr, collision_info);
            }

            for (auto object_state = state_buffer_.begin(); !collision_point && object_state != state_buffer_.end(); ++object_state)
            {
                auto object = object_state->entity;
                if (object == subject || object_state->level != subject_state.level)
                    continue;

                const auto& object_bitmap = object->collision_bitmap();
                if (collision_point = collision_test(collision_bitmap, object_bitmap, step.new_position, object_state->current_position,
                    subject_state.rotation_index, object_state->rotation_index))
                {
                    auto collision_info = resolve_entity_collision(collision_point.point, subject, object, step.new_position, object_state->current_position);
                    handle_collision(subject, object, collision_info);

                    resolve_recurring_collisions(subject_state, *object_state, fd);
                    resolve_recurring_collisions(*object_state, subject_state, fd);
                }
            }

            if (collision_point)
            {
                target_position.x = subject_state.current_position.x + (target_position.x - std::floor(target_position.x));
                target_position.y = subject_state.current_position.y + (target_position.y - std::floor(target_position.y));
                break;
            }
        }

        subject->set_position(target_position);
        subject->set_rotation(target_rotation);

        subject_state.current_position = static_cast<Vector2i>(target_position);
    }

    for (auto listener : world_listeners_)
    {
        listener->on_update();
    }

    auto old_game_time = game_timer_.time();
    game_timer_.update(frame_duration);
    world_time_ += frame_duration;

    for (auto listener : world_listeners_)
    {
        listener->on_tick(game_timer_.time());
    }
}



void ts::world::World::terrain_transition(Entity_update_state& state, const resources::Terrain_definition& old_terrain,
                                          const resources::Terrain_definition& new_terrain)
{
    const auto& terrain_library = track_->terrain_library();
    const auto entity = state.entity;

    auto old_level = state.level;
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
        state.level = new_level;
        if (entity_state_collides(state))
        {
            // Cannot put the entity on the new level
            state.level = old_level;
        }

        else
        {
            entity->set_z_position(static_cast<double>(state.level));
        }
    }
}

void ts::world::World::handle_collision(Entity* subject, Entity* object, const Collision_info& collision_info)
{
    for (auto listener : world_listeners_)
    {
        listener->on_collision(subject, object, collision_info);
    }
}

bool ts::world::World::entity_state_collides(const Entity_update_state& state)
{
    const auto entity = state.entity;
    const auto& collision_bitmap = entity->collision_bitmap();

    if (collision_test(collision_bitmap, scenery_bitmap_, state.current_position, state.rotation_index, state.level))
    {
        return true;
    }

    for (const auto& other_state : state_buffer_)
    {
        auto other = other_state.entity;
        if (other == entity || other_state.level != state.level) continue;                   
                
        if (collision_test(collision_bitmap, other->collision_bitmap(),
            state.current_position, other_state.current_position, state.rotation_index, other_state.rotation_index))
        {
            return true;
        }
    }

    return false;
}

bool ts::world::World::resolve_recurring_collisions(Entity_update_state& subject_state, Entity_update_state& object_state, double frame_duration)
{
    const auto subject = subject_state.entity;
    const auto object = object_state.entity;

    Vector2i subject_target_position = compute_new_position(*subject, frame_duration);
    Vector2i object_target_position = compute_new_position(*object, frame_duration);

    auto get_new_step_position = [](Vector2i current_position, Vector2i target_position)
    {
        Vector2i offset = target_position - current_position, result = current_position;
        std::int32_t x_offset = std::abs(offset.x), y_offset = std::abs(offset.y);

        if (x_offset != 0 && x_offset >= y_offset) result.x += offset.x > 0 ? 1 : -1;
        if (y_offset != 0 && y_offset >= x_offset) result.y += offset.y > 0 ? 1 : -1;

        return result;
    };

    auto relative_offset = subject_state.current_position - object_state.current_position;

    auto subject_dummy_state = subject_state;
    subject_dummy_state.current_position = get_new_step_position(subject_state.current_position, subject_target_position);

    auto object_dummy_state = object_state;
    object_dummy_state.current_position = get_new_step_position(object_state.current_position, object_target_position);
    
    const auto& subject_bitmap = subject->collision_bitmap();
    const auto& object_bitmap = object->collision_bitmap();

    if (collision_test(subject_bitmap, object_bitmap, subject_dummy_state.current_position, object_state.current_position,
        subject_dummy_state.rotation_index, object_state.rotation_index))
    {
        Vector2i avoidance(relative_offset.x >= 0 ? -1 : 1, relative_offset.y >= 0 ? -1 : 1);
        Vector2i avoidance_attempts[2] = { Vector2i(avoidance.x, 0), Vector2i(0, avoidance.y) };
        if (std::abs(relative_offset.x) < std::abs(relative_offset.y))
        {
            std::swap(avoidance_attempts[0], avoidance_attempts[1]);
        }

        auto collision_point = collision_test(object_bitmap, scenery_bitmap_, object_dummy_state.current_position, object_dummy_state.rotation_index, object_dummy_state.level);
        if (entity_state_collides(object_dummy_state))
        {
            auto avoidance_result = std::find_if(std::begin(avoidance_attempts), std::end(avoidance_attempts), 
                                                 [this, object_state](Vector2i attempt)
            {
                auto avoidance_state = object_state;
                avoidance_state.current_position += attempt;

                return !entity_state_collides(avoidance_state);
            });

            if (avoidance_result != std::end(avoidance_attempts))
            {
                object_state.current_position += *avoidance_result;

                Vector2<double> new_position = object_state.current_position;
                new_position.x += object_target_position.x - std::floor(object_target_position.x);
                new_position.y += object_target_position.y - std::floor(object_target_position.y);
                object->set_position(new_position);
            }
        }
    }

    return false;
}

// For a moving entity, we need to pass through all terrains between
// its old and new position.
void ts::world::World::displacement_step(Entity_update_state& state, Vector2i old_position, Vector2i new_position)
{
    const auto& old_terrain = terrain_at(old_position);
    const auto& new_terrain = terrain_at(new_position);

    if (old_terrain != new_terrain)
    {
        terrain_transition(state, old_terrain, new_terrain);
    }
}

double ts::world::World::max_entity_speed()
{
    return world::max_entity_speed_config;
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
    return *track_;
}

const std::vector<std::unique_ptr<ts::world::Car>>& ts::world::World::car_list() const
{
    return car_list_;
}

const std::vector<ts::world::Entity*>& ts::world::World::entity_list() const
{
    return entity_list_;
}