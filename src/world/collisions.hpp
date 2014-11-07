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
        struct Collision_info
        {
            Vector2<std::int32_t> point;
            Vector2<double> normal;
            double impact = 0.0;
            double elasticity_factor = 0.0;
        };


        class World;
        class Static_collision_bitmap;

        struct Entity_state;

        //Entity_state displace_unrotated_entity(const Collision_result& collision_result, const Static_collision_bitmap& scenery);

        Collision_info resolve_entity_collision(Vector2i collision_point, Entity* subject, Entity* object,
                                      Vector2i subject_position, Vector2i object_position);

        Collision_info resolve_entity_collision(Vector2i collision_point, Entity* subject, Entity* object,
                                      Vector2i subject_position, Vector2i object_position, Vector2<double> normal);

        Collision_info resolve_scenery_collision(Vector2i collision_point, const Static_collision_bitmap& scenery,
                                       Entity* subject, Vector2i subject_position, double bounce_factor);

    }
}


#endif