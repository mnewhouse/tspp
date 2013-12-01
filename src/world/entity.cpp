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

#include "entity.hpp"
#include "world.hpp"

#include "resources/collision_mask.hpp"
#include "resources/terrain_definition.hpp"

#include "collisions.hpp"

ts::world::Entity::Entity(World* world, const resources::Collision_mask& collision_mask)
    : world_(world),
      collision_mask_(collision_mask),
      position_(),
      rotation_(),
      velocity_(),
      angular_velocity_(),
      z_position_()
{
}


ts::world::Entity::~Entity()
{
}

double ts::world::Entity::z_position() const
{
    return z_position_;
}

void ts::world::Entity::set_z_position(double z_position)
{
    z_position_ = z_position;
}

const ts::Vector2d& ts::world::Entity::position() const
{
    return position_;
}


ts::Rotation<double> ts::world::Entity::rotation() const
{
    return rotation_;
}

const ts::Vector2d& ts::world::Entity::velocity() const
{
    return velocity_;
}

double ts::world::Entity::angular_velocity() const
{
    return angular_velocity_;
}

void ts::world::Entity::set_velocity(Vector2d velocity)
{
    velocity_ = velocity;
}

void ts::world::Entity::set_angular_velocity(double angular_velocity)
{
    angular_velocity_ = angular_velocity;
}

void ts::world::Entity::set_position(Vector2d position)
{
    position_ = position;
}

void ts::world::Entity::set_rotation(Rotation<double> rotation)
{
    rotation_ = rotation;
}

const ts::resources::Terrain_definition& ts::world::Entity::current_terrain() const
{
    auto level = static_cast<int>(z_position());
    return world_->terrain_at(position(), level);
}

const ts::resources::Collision_mask& ts::world::Entity::collision_mask() const
{
    return collision_mask_;
}