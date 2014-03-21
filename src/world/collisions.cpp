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

#include "collisions.hpp"
#include "collision_helpers.hpp"

#include <fstream>

namespace ts
{
    namespace world
    {
        Vector2<double> adjust_position(const Vector2<double>& position);
    }
}

ts::Vector2<double> ts::world::adjust_position(const Vector2<double>& position)
{    
    auto target_position = position;
    target_position.x = std::floor(target_position.x) + 0.5;
    target_position.y = std::floor(target_position.y) + 0.5;    
    return target_position;
}

ts::world::Collision_result ts::world::detect_entity_collision(Entity_state subject_state, Entity_state object_state)
{
    auto subject = subject_state.entity;
    auto object = object_state.entity;

    Collision_result result;
    if (subject == object || object->z_level() != subject->z_level()) return result;

    auto subject_offset = subject_state.position - subject->position();
    auto object_offset = object_state.position - object->position();
    auto relative_offset = subject_offset - object_offset;

    auto subject_heading = normalize(subject->velocity());
    auto object_heading = normalize(object->velocity());

    /*

    {
        auto position_diff = subject->position() - object->position();
        auto subject_dot = dot_product(-position_diff, subject_heading);
        auto object_dot = dot_product(position_diff, object_heading);

        if (subject_dot < object_dot)
        {
            std::swap(subject_state, object_state);
            std::swap(subject, object);
            std::swap(subject_offset, object_offset);
            std::swap(subject_heading, object_heading);

            relative_offset = -relative_offset;
        }
    }
    */

    const auto& subject_bitmap = subject->collision_bitmap();
    const auto& object_bitmap = object->collision_bitmap();

    auto trajectory_test = [&](const Vector2<double>& offset, double time_point)
    {
        auto object_position = interpolate_position(object->position(), object_state.position, time_point);
        auto subject_position = interpolate_position(subject->position(), subject_state.position, time_point);

        return collision_test(subject_bitmap, object_bitmap,
                              subject_position, object_position,
                              subject_state.rotation, object_state.rotation);

    };

    auto collision_point = test_trajectory({}, subject_offset, trajectory_test);
    if (!collision_point.result) return result;

    result.collided = true;
    result.rotate = true;
    result.deflect = true;
    result.time_point = collision_point.valid_time_point;
    result.global_point = collision_point.result.point;

    result.subject_state = subject_state;
    result.object_state = object_state;

    if (result.time_point < 0.0)
    {
        if (subject_state.rotation.radians() == subject->rotation().radians() &&
            object_state.rotation.radians() == object->rotation().radians())
        {
            result.stuck = true;
        }

        result.subject_state.rotation = subject->rotation();
        result.object_state.rotation = object->rotation();

        result.deflect = false;
        result.rotate = false;
    }

    {
        auto time_point = std::max(result.time_point, 0.0);

        auto subject_position = interpolate_position(subject->position(), subject_state.position, time_point);
        auto object_position = interpolate_position(object->position(), object_state.position, time_point);

        result.subject_state.position = adjust_position(subject_position);
        result.object_state.position = adjust_position(object_position);
    }

    {
        Vector2i collision_position = interpolate_position(object->position(), object_state.position, collision_point.time_point);
        Vector2i local_point = result.global_point - collision_position;

        auto is_wall = [&](Vector2i point)
        {
            return object_bitmap(point, object_state.rotation);
        };

        result.normal = get_edge_normal(local_point, relative_offset, is_wall);
    }

    {
        Vector2i collision_position = interpolate_position(subject->position(), subject_state.position, collision_point.time_point);
        Vector2i local_point = result.global_point - collision_position;      

        auto is_wall = [&](Vector2i point)
        {
            return subject_bitmap(point, subject_state.rotation);
        }; 

        auto normal = get_edge_normal(local_point, -relative_offset, is_wall);

        if (subject->mass() > object->mass())
        {
            if (dot_product(subject_heading, normalize(relative_offset)) > std::sqrt(0.5)) 
                result.normal = subject_heading;

            else result.normal = normal;
        }

        else
        {
            if (dot_product(object_heading, normalize(-relative_offset)) > std::sqrt(0.5))
                result.normal = object_heading;
        }
    }

    return result;
}

ts::world::Collision_result ts::world::detect_scenery_collision(const Entity_state& entity_state, const Static_collision_bitmap& scenery)
{
    auto entity = entity_state.entity;
    auto level = entity->z_level();

    const auto& bitmap = entity->collision_bitmap();

    auto trajectory_test = [&](const Vector2<double>& position, double time_point)
    {
        return collision_test(bitmap, scenery, position, entity_state.rotation, level);
    };

    Collision_result result;
    auto collision_point = test_trajectory(entity->position(), entity_state.position, trajectory_test);
    if (!collision_point.result) return result;

    auto is_wall = [&](Vector2u point)
    {
        return scenery(point, level);
    };

    auto heading = normalize(entity->velocity());

    result.rotate = true;
    result.deflect = true;
    result.collided = true;
    result.time_point = collision_point.valid_time_point;
    result.global_point = collision_point.result.point;

    result.subject_state = entity_state;
    result.subject_state.position = adjust_position(collision_point.valid_point);
    result.subject_state.rotation = entity_state.rotation;    

    if (result.time_point < 0.0)
    {
        if (entity_state.rotation.radians() == entity->rotation().radians())
        {
            result.stuck = true;
        }

        result.subject_state.rotation = entity->rotation();
        result.rotate = false;
        result.deflect = false;
    }

    result.normal = get_edge_normal(result.global_point, heading, is_wall);
    
    return result;    
}

void ts::world::resolve_scenery_collision(const Collision_result& collision, double wall_bounciness)
{
    // Only update the velocity if 'deflect' flag is set
    if (collision.deflect)
    {
        const auto& subject_state = collision.subject_state;
        auto entity = subject_state.entity;        

        const auto& normal = collision.normal;
        const auto& velocity = entity->velocity();

        auto new_velocity = -(2.0 * dot_product(velocity, normal) * normal - velocity);
        auto bounciness_factor = entity->elasticity() * wall_bounciness;

        entity->set_velocity(new_velocity * bounciness_factor);
    }
}

void ts::world::resolve_entity_collision(const Collision_result& collision)
{
    if (collision.deflect)
    {
        auto subject = collision.subject_state.entity;
        auto object = collision.object_state.entity;

        const auto& normal = collision.normal;

        auto bounciness_factor = subject->elasticity() * object->elasticity();

        auto subject_mass = std::max(subject->mass(), 1.0);
        auto object_mass = std::max(object->mass(), 1.0);

        auto total_mass = subject_mass + object_mass;

        auto subject_velocity = subject->velocity();
        auto object_velocity = object->velocity();

        auto subject_heading = normalize(subject_velocity);
        auto object_heading = normalize(object_velocity);
                                    
        auto subject_speed_1d = dot_product(subject_velocity, normal);
        auto object_speed_1d = dot_product(object_velocity, normal);

        auto subject_momentum = subject_speed_1d * subject_mass;
        auto object_momentum = object_speed_1d * object_mass;

        auto total_momentum = subject_momentum + object_momentum;

        auto factor = (1.0 / total_mass) * bounciness_factor;
        auto inverse_bounce = (total_momentum / total_mass) * (1.0 - bounciness_factor);

        auto new_subject_speed_1d = (subject_momentum - subject_speed_1d * object_mass +
                                     object_speed_1d * (object_mass + object_mass)) *
                                     factor + inverse_bounce;

        auto new_object_speed_1d = (object_momentum + subject_speed_1d *
                                    (subject_mass + subject_mass) - object_speed_1d * subject_mass) *
                                    factor + inverse_bounce;

        subject->set_velocity(subject_velocity + normal * (new_subject_speed_1d - subject_speed_1d));
        object->set_velocity(object_velocity + normal * (new_object_speed_1d - object_speed_1d));
    }
}
