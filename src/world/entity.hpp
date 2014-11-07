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

#ifndef WORLD_ENTITY_HPP
#define WORLD_ENTITY_HPP

#include "collision_bitmap.hpp"

namespace ts
{
    namespace resources
    {
        class Pattern;
        struct Terrain_definition;
    }

    namespace world
    {
        class World;

        enum class Locality
        {
            Local,
            Remote
        };

        class Entity
        {
        public:
            Entity(World* world, const std::shared_ptr<Collision_bitmap>& collision_bitmap);

            virtual ~Entity() = 0;

            Rotation<double> rotation() const;
            const Vector2d& position() const;
            const Vector2d& velocity() const;
            double angular_velocity() const;

            double speed() const;

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

            void set_elasticity(double elasticity);
            double elasticity() const;

            std::uint32_t z_level() const;

            double mass() const;
            void set_mass(double mass);        

            const Collision_bitmap& collision_bitmap() const;
            Collision_bitmap& collision_bitmap();

            Locality locality() const;
            void set_locality(Locality locality);

        private:
            World* world_;

            Vector2d position_;            
            Rotation<double> rotation_;
            double z_position_;

            Vector2d velocity_;
            double angular_velocity_;

            double elasticity_;
            double mass_;

            Locality locality_;

            std::shared_ptr<Collision_bitmap> collision_bitmap_;
        };

        struct Entity_state
        {
            Entity* entity = nullptr;
            Vector2<double> position;
            Rotation<double> rotation;

            Vector2<double> velocity;
            double angular_velocity;
            double z_position;
        };

        void apply_state(const Entity_state& state);

    }

}

#endif