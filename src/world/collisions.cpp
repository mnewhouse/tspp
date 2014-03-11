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

ts::world::Collision_result ts::world::detect_scenery_collision(const Entity_state& entity_state, const Static_collision_bitmap& scenery)
{
    auto entity = entity_state.entity;
    auto level = entity->z_level();

    const auto& bitmap = entity->collision_bitmap();

    auto trajectory_test = [&](const Vector2<double>& position, double time_point)
    {
        return collision_test(bitmap, scenery, position, level);
    };

    Collision_result result;
    auto collision_point = test_trajectory(entity->position(), entity_state.position, trajectory_test);
    if (!collision_point.result) return result;

    auto is_wall = [&](Vector2u point)
    {
        return scenery(point, level);
    };

    result.rotate = true;
    result.deflect = true;
    result.collided = true;
    result.time_point = collision_point.valid_time_point;
    result.global_point = collision_point.result.point;

    result.subject_state = entity_state;
    result.subject_state.position = collision_point.valid_point;
    result.subject_state.rotation = entity_state.rotation;

    if (collision_point.valid_time_point < 0.0)
    {
        result.subject_state.rotation = entity->rotation();
        result.rotate = false;
        result.deflect = false;
        result.time_point = 0.0;
    }

    result.normal = get_edge_normal(result.global_point, entity->velocity(), is_wall);
    
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