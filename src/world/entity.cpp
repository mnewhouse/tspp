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
#include "entity.hpp"
#include "world.hpp"
#include "collision_bitmap.hpp"

#include "resources/terrain_definition.hpp"

ts::world::Entity::Entity(World* world, const std::shared_ptr<resources::Pattern>& pattern)
    : world_(world),
      position_(0, 0),
      rotation_(),
      z_position_(0),
      velocity_(0, 0),
      angular_velocity_(0),      
      collision_bitmap_(world->collision_bitmap(pattern))
{
}


ts::world::Entity::~Entity()
{
}

double ts::world::Entity::z_position() const
{
    return z_position_;
}

std::uint32_t ts::world::Entity::z_level() const
{
  return static_cast<std::uint32_t>(z_position_);
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

ts::world::Locality ts::world::Entity::locality() const
{
    return locality_;
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

void ts::world::Entity::set_locality(Locality locality)
{
    locality_ = locality;
}

const ts::resources::Terrain_definition& ts::world::Entity::current_terrain() const
{
    auto level = static_cast<int>(z_position());
    return world_->terrain_at(position(), level);
}

const ts::world::Collision_bitmap& ts::world::Entity::collision_bitmap() const
{
    return *collision_bitmap_;
}

ts::world::Collision_bitmap& ts::world::Entity::collision_bitmap()
{
    return *collision_bitmap_;
}

void ts::world::Entity::set_elasticity(double elasticity)
{
    elasticity_ = elasticity;
}

double ts::world::Entity::elasticity() const
{
    return elasticity_;
}

void ts::world::Entity::set_mass(double mass)
{
    mass_ = mass;
}

double ts::world::Entity::mass() const
{
    return mass_;
}

double ts::world::Entity::speed() const
{
    return magnitude(velocity_);
}

void ts::world::apply_state(const Entity_state& state)
{
    auto entity = state.entity;

    entity->set_position(state.position);
    entity->set_rotation(state.rotation);
    entity->set_velocity(state.velocity);
    entity->set_angular_velocity(state.angular_velocity);
    entity->set_z_position(state.z_position);
}