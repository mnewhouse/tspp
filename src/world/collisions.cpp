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

namespace ts
{
    namespace world
    {
        Vector2<double> adjust_position(const Vector2<double>& position, const Vector2<double>& heading);
    }
}

ts::Vector2<double> ts::world::adjust_position(const Vector2<double>& position, const Vector2<double>& heading)
{    
    auto target_position = position;
    if (std::abs(heading.x) > std::abs(heading.y))
    {
        target_position.x = std::floor(target_position.x) + 0.5;
        if (heading.x > 0.0) target_position.x = std::min(target_position.x, position.x);
        else target_position.x = std::max(target_position.x, position.x);

        target_position.y += (target_position.x - position.x) / heading.x * heading.y;
    }
        
    else if (heading.y != 0.0)
    {
        target_position.y = std::floor(target_position.y) + 0.5;
        if (heading.y > 0.0) target_position.y = std::min(target_position.y, position.y);
        else target_position.y = std::max(target_position.y, position.y);

        target_position.x += (target_position.y - position.y) / heading.y * heading.x;
    }

    return target_position;
}

ts::world::Collision_result ts::world::detect_entity_collision(const Entity_state& subject_state, const Entity_state& object_state)
{
    auto subject = subject_state.entity;
    auto object = object_state.entity;

    const auto& subject_bitmap = subject->collision_bitmap();
    const auto& object_bitmap = object->collision_bitmap();

    Collision_result result;

    auto subject_level = subject->z_level();
    auto object_level = object->z_level();
    if (subject == object || subject_level != object_level) return result;

    auto subject_offset = subject_state.position - subject->position();
    auto object_offset = object_state.position - object->position();

    auto trajectory_test = [&](const Vector2<double> offset, double time_point)
    {
        auto object_position = interpolate_position(object->position(), object_state.position, time_point);
        auto subject_position = interpolate_position(subject->position(), subject_state.position, time_point);

        return collision_test(subject_bitmap, object_bitmap, 
                              subject_position, object_position, 
                              subject_state.rotation, object_state.rotation,
                              subject_level, object_level);
    };

    auto collision_point = test_trajectory({ 0.0, 0.0 }, subject_offset - object_offset, trajectory_test);
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
        result.subject_state.rotation = subject->rotation();
        result.object_state.rotation = object->rotation();

        result.time_point = 0.0;
        result.deflect = false;
        result.rotate = false;
    }

    result.subject_state.position = interpolate_position(subject->position(), subject_state.position, result.time_point);
    result.object_state.position = interpolate_position(object->position(), object_state.position, result.time_point);

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
    result.subject_state.position = adjust_position(collision_point.valid_point, heading);
    result.subject_state.rotation = entity_state.rotation;    

    if (collision_point.valid_time_point < 0.0)
    {
        result.subject_state.rotation = entity->rotation();
        result.rotate = false;
        result.deflect = false;
        result.time_point = 0.0;
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