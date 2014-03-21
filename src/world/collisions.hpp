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

#ifndef WORLD_COLLISIONS_HPP
#define WORLD_COLLISIONS_HPP

#include "core/vector2.hpp"
#include "entity.hpp"

namespace ts
{
    namespace world
    {
        struct Collision_result
        {
            bool collided = false;
            bool rotate = false;
            bool deflect = false;
            bool stuck = false;

            Entity_state subject_state;
            Entity_state object_state;            

            double time_point = 0.0;
            Vector2<double> normal;
            Vector2<int> global_point;

            operator bool() const { return collided; }
        };

        struct Entity_state;
        class Static_collision_bitmap;

        template <typename InputIt>
        Collision_result detect_entity_collision(const Entity_state& subject, InputIt it, InputIt end);

        Collision_result detect_entity_collision(Entity_state subject, Entity_state object);
        Collision_result detect_scenery_collision(const Entity_state& entity_state, const Static_collision_bitmap& scenery);

        void resolve_entity_collision(const Collision_result& collision);
        void resolve_scenery_collision(const Collision_result& collision, double wall_bounciness);
    }
}

template <typename InputIt>
ts::world::Collision_result ts::world::detect_entity_collision(const Entity_state& subject, InputIt it, InputIt end)
{
    if (it == end) return {};

    auto collision = detect_entity_collision(subject, *it);
    while (++it != end)
    {
        auto new_collision = detect_entity_collision(subject, *it);
        if (new_collision && (!collision || new_collision.time_point < collision.time_point))
        {
            collision = new_collision;
        }
    }

    return collision;
}


#endif