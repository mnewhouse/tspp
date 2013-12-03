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

#ifndef WORLD_ENTITY_HPP
#define WORLD_ENTITY_HPP


#include "core/rotation.hpp"
#include "core/vector2.hpp"

#include "resources/terrain_definition.hpp"
#include "resources/pattern.hpp"

#include <memory>

namespace ts
{
    namespace resources {
        struct Collision_mask;
    }

    namespace world
    {
        class World;

        class Entity
        {
        public:
            Entity(World* world, const resources::Collision_mask& collision_mask);

            virtual ~Entity() = 0;

            Rotation<double> rotation() const;
            const Vector2d& position() const;
            const Vector2d& velocity() const;
            double angular_velocity() const;

            void set_rotation(Rotation<double> rotation);
            void set_position(Vector2d position);
            void set_velocity(Vector2d velocity);
            void set_angular_velocity(double angular_velocity);

            void apply_force(Vector2d force);
            void apply_torque(double torque);

            virtual void update(double frame_duration) = 0;

            const resources::Terrain_definition& current_terrain() const;

            void set_z_position(double z_position);
            double z_position() const;

            const resources::Collision_mask& collision_mask() const;

        private:
            World* world_;

            Vector2d position_;            
            Rotation<double> rotation_;
            double z_position_;

            Vector2d velocity_;
            double angular_velocity_;            

            resources::Collision_mask collision_mask_;
        };

    }

}

#endif