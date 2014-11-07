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

        struct Safe_wall_test
        {
        public:
            Safe_wall_test(const Entity* entity)
                : collision_bitmap_(entity->collision_bitmap()),
                  bitmap_size_(collision_bitmap_.size()),
                  bitmap_center_(bitmap_size_.x >> 1, bitmap_size_.y >> 1),
                  rotation_index_(collision_bitmap_.rotation_index(entity->rotation()))
            {
            }

            bool operator()(Vector2i point) const
            {
                point += bitmap_center_;

                return point.x >= 0 && point.y >= 0 && point.x < bitmap_size_.x && point.y < bitmap_size_.y &&
                    collision_bitmap_(point, rotation_index_);
            }

        private:
            const Collision_bitmap& collision_bitmap_;
            Vector2i bitmap_size_;
            Vector2i bitmap_center_;
            std::size_t rotation_index_;            
        };

        struct Safe_scenery_wall_test
        {
        public:
            Safe_scenery_wall_test(const Static_collision_bitmap& scenery_bitmap, std::size_t level)
                : scenery_bitmap_(scenery_bitmap),
                  scenery_size_(scenery_bitmap.size()),
                  level_(level)
            {}


            bool operator()(Vector2i point) const
            {
                return point.x >= 0 && point.y >= 0 && point.x < scenery_size_.x && point.y < scenery_size_.y && 
                    scenery_bitmap_(point, level_);
            }

        private:
            const Static_collision_bitmap& scenery_bitmap_;
            Vector2i scenery_size_;
            std::size_t level_;
        };
    }
}

ts::Vector2<double> ts::world::adjust_position(Vector2<double> position)
{    
    position.x = std::floor(position.x) + 0.5;
    position.y = std::floor(position.y) + 0.5;    
    return position;
}

ts::world::Collision_info ts::world::resolve_entity_collision(Vector2i collision_point, Entity* subject, Entity* object,
                                         Vector2i subject_position, Vector2i object_position)
{
    const auto subject_velocity = subject->velocity();
    const auto object_velocity = object->velocity();

    const auto relative_velocity = subject_velocity - object_velocity;

    Vector2i subject_point = collision_point - subject_position;
    Vector2i object_point = collision_point - object_position;

    auto calculate_normal = [=]()
    {
        auto offset_vector = normalize(subject->position() - object->position());

        if (dot_product(subject_velocity, relative_velocity) >= dot_product(object_velocity, -relative_velocity))
        {
            auto subject_heading = normalize(subject_velocity);
            return dot_product(subject_heading, -offset_vector) >= 0.70 ? subject_heading : offset_vector;
        }

        else
        {
            auto object_heading = normalize(object_velocity);
            return dot_product(object_heading, offset_vector) >= 0.70 ? object_heading : -offset_vector;
        }
    };

    return resolve_entity_collision(collision_point, subject, object, subject_position, object_position, calculate_normal());
}

ts::world::Collision_info ts::world::resolve_entity_collision(Vector2i collision_point, Entity* subject, Entity* object,
                                         Vector2i subject_position, Vector2i object_position, Vector2<double> normal)
{
    const auto subject_velocity = subject->velocity();
    const auto object_velocity = object->velocity();

    const auto relative_velocity = subject_velocity - object_velocity;

    auto apply_spin = [](Entity* entity, Vector2<double> local_point, Vector2<double> relative_velocity)
    {
        Vector2<double> center_axis = normalize(Vector2<double>(-relative_velocity.y, relative_velocity.x)), intersection;
        if (line_intersection(local_point, local_point + relative_velocity, Vector2<double>(), center_axis, intersection))
        {
            entity->apply_torque(dot_product(intersection, center_axis));
        }
    };

    double subject_mass = std::max(subject->mass(), 1.0);
    double object_mass = std::max(object->mass(), 1.0);
    double total_mass = subject_mass + object_mass;

    auto relative_offset = subject->position() - object->position();
    auto deflection_boost = normalize(relative_offset) * 10.0;

    auto subject_speed_1d = dot_product(subject_velocity, normal);
    auto object_speed_1d = dot_product(object_velocity, normal);
    auto relative_speed_1d = subject_speed_1d - object_speed_1d;

    Collision_info collision_info;
    collision_info.normal = normal;
    collision_info.elasticity_factor = subject->elasticity() * object->elasticity();
    collision_info.impact = std::abs(relative_speed_1d);
    collision_info.point = collision_point;

    auto new_relative_speed_1d = -relative_speed_1d * collision_info.elasticity_factor;

    Vector2i subject_point = collision_point - subject_position;
    Vector2i object_point = collision_point - object_position;

    apply_spin(subject, subject_point, relative_velocity);
    apply_spin(object, object_point, -relative_velocity);

    auto transferred_force = (normal * (new_relative_speed_1d - relative_speed_1d) * subject_mass * object_mass) / total_mass;
    subject->apply_force(transferred_force + deflection_boost * subject_mass);
    object->apply_force(-transferred_force - deflection_boost * object_mass);

    return collision_info;
}

ts::world::Collision_info ts::world::resolve_scenery_collision(Vector2i collision_point, const Static_collision_bitmap& scenery,
                                          Entity* subject, Vector2i subject_position, double bounce_factor)
{
    auto velocity = subject->velocity();

    Collision_info result;
    result.point = collision_point;
    result.normal = get_edge_normal(collision_point, velocity, Safe_scenery_wall_test(scenery, subject->z_level()));
    result.impact = std::abs(dot_product(velocity, result.normal));
    result.elasticity_factor = subject->elasticity() * bounce_factor;

    assert(Safe_scenery_wall_test(scenery, subject->z_level())(collision_point));

    auto new_velocity = -(2.0 * dot_product(velocity, result.normal) * result.normal - velocity) * result.elasticity_factor;    

    subject->set_velocity(new_velocity);

    return result;    
}
