/*
 * Turbo Sliders++
 * Copyright (C) 2013 Martin Newhouse
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

#include "resources/pattern.hpp"
#include "core/rotation.hpp"

#include "entity.hpp"

namespace ts
{
    namespace world
    {
        struct Collision_result
        {
            bool collided;
            bool stuck;
            bool rotate;

            double bounciness;

            Vector2<double> normal;
            double time_point;

            Entity* subject;
            Entity* object;

            Vector2<double> subject_position;
            Vector2<double> object_position;

            Rotation<double> subject_rotation;
            Rotation<double> object_rotation;
        };

        Collision_result detect_collision(const Entity_state& subject, const resources::Pattern& scenery, const resources::Terrain_library& terrain_lib);

        Collision_result detect_collision(Entity_state subject, Entity_state object);
    }
}



#endif
