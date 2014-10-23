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
#include "collisions.hpp"
#include "collision_helpers.hpp"

#include "world/world.hpp"

namespace ts
{
    namespace world
    {
        Vector2<double> adjust_position(Vector2<double> position);
    }
}

ts::Vector2<double> ts::world::adjust_position(Vector2<double> position)
{    
    position.x = std::floor(position.x) + 0.5;
    position.y = std::floor(position.y) + 0.5;    
    return position;
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
    result.valid_time_point = collision_point.valid_time_point;
    result.time_point = collision_point.time_point;
    result.global_point = collision_point.result.point;
    result.impact = magnitude(subject->velocity() - object->velocity());

    result.subject_state = subject_state;
    result.object_state = object_state;

    if (result.valid_time_point < 0.0)
    {
        result.stuck = collision_test(subject_bitmap, object_bitmap, subject->position(), object->position(), 
                                      subject->rotation(), object->rotation());

        result.rotate = false;
    }

    {
        auto time_point = std::max(collision_point.valid_time_point, 0.0);

        auto subject_position = interpolate_position(subject->position(), subject_state.position, time_point);
        auto object_position = interpolate_position(object->position(), object_state.position, time_point);

        Vector2i subject_collision_position = interpolate_position(subject->position(), subject_state.position, collision_point.time_point);
        Vector2i object_collision_position = interpolate_position(object->position(), object_state.position, collision_point.time_point);

        result.subject_state.position = adjust_position(subject_position);
        result.object_state.position = adjust_position(object_position);

        result.subject_point = subject_collision_position - result.global_point;
        result.object_point = object_collision_position - result.global_point;
    }

    auto calculate_normal = [](const Entity_state& entity_state, Vector2<double> relative_offset, Vector2i global_point, double time_point)
    {
        auto entity = entity_state.entity;
        auto rotation = entity_state.rotation;

        const auto& bitmap = entity->collision_bitmap();

        auto is_wall = [&bitmap, rotation](Vector2i point)
        {
            return bitmap(point, rotation);
        };

        Vector2i collision_position = interpolate_position(entity->position(), entity_state.position, time_point);
        Vector2i local_point = global_point - collision_position;

        auto normal = get_edge_normal(local_point, relative_offset, is_wall);
        auto heading = normalize(entity->velocity());

        if (dot_product(normal, heading) > 0.5)
        {
            normal = heading;
        }

        return normal;
    };

    if (dot_product(subject_offset, relative_offset) >= dot_product(object_offset, -relative_offset))
    {
        result.normal = calculate_normal(subject_state, -relative_offset, result.global_point, result.time_point);
    }

    else
    {
        result.normal = calculate_normal(object_state, relative_offset, result.global_point, result.time_point);
    }

    std::tie(result.subject_state, result.object_state) = resolve_entity_collision(result);

    return result;
}

ts::world::Collision_result ts::world::detect_scenery_collision(const Entity_state& entity_state, const world::World& world, 
    const Static_collision_bitmap& scenery)
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

    auto safe_is_wall = [&](Vector2u point)
    {
        return point.x < scenery.size().x && point.y < scenery.size().y && scenery(point, level);
    };

    auto heading = normalize(entity->velocity());

    result.rotate = true;
    result.collided = true;
    result.valid_time_point = collision_point.valid_time_point;
    result.time_point = collision_point.time_point;
    result.global_point = collision_point.result.point;
    result.impact = magnitude(entity->velocity());


    auto terrain = world.terrain_at(result.global_point, level);
    result.elasticity_factor = terrain.wall_definition.elasticity;    

    result.subject_state = entity_state;
    result.subject_state.position = adjust_position(collision_point.valid_point);

    if (result.valid_time_point < 0.0)
    {
        result.stuck = collision_test(bitmap, scenery, entity->position(), entity->rotation(), level);
        result.rotate = false;
    }

    result.normal = get_edge_normal(result.global_point, heading, safe_is_wall);
    result.subject_state = resolve_scenery_collision(result);
    result.subject_point = result.global_point - Vector2i(collision_point.point);
    
    return result;    
}


ts::world::Entity_state ts::world::resolve_scenery_collision(const Collision_result& collision)
{
    Entity_state result = collision.subject_state;

    const auto& subject_state = collision.subject_state;
    auto entity = subject_state.entity;

    const auto normal = collision.normal;
    const auto velocity = entity->velocity();

    auto new_velocity = -(2.0 * dot_product(velocity, normal) * normal - velocity);
    auto bounciness_factor = entity->elasticity() * collision.elasticity_factor;        

    result.velocity = new_velocity * bounciness_factor;

    return result;
}

ts::world::Entity_state ts::world::displace_unrotated_entity(const Collision_result& collision, const Static_collision_bitmap& scenery)
{
    auto result = collision.subject_state;
    auto entity = result.entity;

    const auto& bitmap = entity->collision_bitmap();
    const auto level = entity->z_level();

    // Get the displacement direction

    Vector2<double> target_point = collision.subject_point;
    Vector2<double> original_point = transform_point(transform_point(target_point, -result.rotation), entity->rotation());

    auto delta_p = target_point - original_point;
    auto vertical_target = result.position;
    auto horizontal_target = result.position;

    const double max_offset = 10.0;

    horizontal_target.x += max_offset * (delta_p.x >= 0.0 ? -1.0 : 1.0);
    vertical_target.y += max_offset * (delta_p.y >= 0.0 ? -1.0 : 1.0);

    auto test_func = [&](Vector2<double> position, double time_point)
    {
        return !collision_test(bitmap, scenery, position, result.rotation, level);
    };

    auto horizontal_point = test_trajectory(result.position, horizontal_target, test_func);
    auto vertical_point = test_trajectory(result.position, vertical_target, test_func);

    auto compare_func = [](const Trajectory_point<bool>& a, const Trajectory_point<bool>& b)
    {
        if (a.result && b.result) return a.time_point < b.time_point;

        return !b.result;
    };

    const auto& trajectory_point = std::min(horizontal_point, vertical_point, compare_func);
    if (trajectory_point.result)
    {
        result.position = trajectory_point.point;
    }

    else
    {
        result.rotation = entity->rotation();
    }
    
    return result;
}

std::pair<ts::world::Entity_state, ts::world::Entity_state> ts::world::resolve_entity_collision(const Collision_result& collision)
{
    auto result = std::make_pair(collision.subject_state, collision.object_state);

    auto subject = collision.subject_state.entity;
    auto object = collision.object_state.entity;

    const auto normal = collision.normal;

    auto bounce_factor = (subject->elasticity() * object->elasticity());
    auto inverse_bounce = 1.0 - bounce_factor;

    auto subject_mass = std::max(subject->mass(), 1.0);
    auto object_mass = std::max(object->mass(), 1.0);

    auto total_mass = subject_mass + object_mass;

    auto subject_velocity = subject->velocity();
    auto object_velocity = object->velocity();

    /*

    auto subject_momentum = subject_velocity * subject_mass;
    auto object_momentum = object_velocity * object_mass;

    auto total_momentum = subject_momentum + object_momentum;

    auto subject_normal_velocity = normal * dot_product(subject_velocity, normal);
    auto object_normal_velocity = normal * dot_product(object_velocity, normal);

    auto inverse_bounce = total_momentum * (1.0 - bounce_factor);

    auto subject_delta = -subject_normal_velocity * object_mass +
        object_normal_velocity * (object_mass + object_mass);

    auto object_delta = -object_normal_velocity * subject_mass +
        subject_normal_velocity * (subject_mass + subject_mass);
        

    auto subject_reflected = 2.0 * dot_product(subject_delta, normal) * normal - subject_delta;
    auto object_reflected = 2.0 * dot_product(object_delta, normal) * normal - object_delta;

    auto new_subject_velocity = ((subject_momentum + subject_delta) * bounce_factor + inverse_bounce) / total_mass;
    auto new_object_velocity = ((object_momentum + object_delta) * bounce_factor + inverse_bounce) / total_mass;

    */
                                    
    auto subject_speed_1d = dot_product(subject_velocity, normal);
    auto object_speed_1d = dot_product(object_velocity, normal);

    auto subject_momentum = subject_speed_1d * subject_mass;
    auto object_momentum = object_speed_1d * object_mass;

    auto total_momentum = subject_momentum + object_momentum;

    auto new_subject_speed_1d = ((subject_momentum - subject_speed_1d * object_mass +
                                    object_speed_1d * (object_mass + object_mass)) *
                                    bounce_factor + (total_momentum * inverse_bounce)) / total_mass;

    auto new_object_speed_1d = ((object_momentum + subject_speed_1d * (subject_mass + subject_mass) -
                                object_speed_1d * subject_mass) *
                                bounce_factor + (total_momentum * inverse_bounce)) / total_mass;

    auto relative_offset = subject->position() - object->position();
    auto deflection_boost = normalize(relative_offset) * 10.0;

    result.first.velocity = subject_velocity - normal * subject_speed_1d + normal * new_subject_speed_1d + deflection_boost;
    result.second.velocity = object_velocity - normal * object_speed_1d + normal * new_object_speed_1d - deflection_boost;

    return result;
}


ts::world::Match_collision::Match_collision(const Entity* entity)
: entity_(entity)
{
}

bool ts::world::Match_collision::operator()(const Collision_result& collision) const
{
    return entity_ != nullptr && (collision.subject_state.entity == entity_ || collision.object_state.entity == entity_);
}