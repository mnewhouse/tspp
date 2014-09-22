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

#pragma once

#ifndef WORLD_COLLISIONS_HPP
#define WORLD_COLLISIONS_HPP

#include "entity.hpp"

namespace ts
{
    namespace world
    {
        struct Collision_result
        {
            bool collided = false;
            bool rotate = false;
            bool stuck = false;

            Entity_state subject_state;
            Entity_state object_state;            

            double valid_time_point = 0.0;
            double time_point = 0.0;
            Vector2<double> normal;
            Vector2<int> global_point;
            Vector2<int> subject_point;
            Vector2<int> object_point;

            double impact = 0.0;
            double elasticity_factor = 1.0;

            operator bool() const { return collided; }
        };

        struct Match_collision
        {
        public:
            Match_collision(const Entity* entity);

            bool operator()(const Collision_result& collision) const;

        private:
            const Entity* entity_;
        };

        class World;
        class Static_collision_bitmap;

        struct Entity_state;

        template <typename InputIt, typename PriorityTest>
        Collision_result detect_entity_collision(const Entity_state& subject, InputIt it, InputIt end, PriorityTest priority_test);

        Collision_result detect_entity_collision(Entity_state subject, Entity_state object);
        Collision_result detect_scenery_collision(const Entity_state& entity_state, const world::World& world,
            const Static_collision_bitmap& scenery);

        Entity_state displace_unrotated_entity(const Collision_result& collision_result, const Static_collision_bitmap& scenery);

        
        std::pair<Entity_state, Entity_state> resolve_entity_collision(const Collision_result& collision);
        Entity_state resolve_scenery_collision(const Collision_result& collision);
    }
}

template <typename InputIt, typename PriorityTest>
ts::world::Collision_result ts::world::detect_entity_collision(const Entity_state& subject, InputIt it, InputIt end, PriorityTest priority_test)
{
    if (it == end) return {};

    auto collision = detect_entity_collision(subject, *it);
    while (++it != end)
    {
        auto new_collision = detect_entity_collision(subject, *it);
        if (priority_test(new_collision, collision))
        {
            collision = new_collision;
        }
    }

    return collision;
}


#endif