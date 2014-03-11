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

#include "car.hpp"
#include "world.hpp"

ts::world::Car::Car(World* world, const resources::Car_definition& car_definition)
    : Entity(world, car_definition.pattern),
      car_definition_(car_definition),
      current_traction_(0.0),
      is_reversing_(false)
{
    set_elasticity(car_definition.wall_definition.elasticity);
    set_mass(car_definition.handling.mass);
}

const ts::resources::Car_definition& ts::world::Car::car_definition() const
{
    return car_definition_;
}

void ts::world::Car::update(double frame_duration)
{
    handling_.update(car_definition_.handling, *this, frame_duration);
}

bool ts::world::Car::is_reversing() const
{
    return is_reversing_;
}

void ts::world::Car::toggle_reversing(bool reversing)
{
    is_reversing_ = reversing;
}

double ts::world::Car::current_traction() const
{
    return current_traction_;
}

void ts::world::Car::set_current_traction(double traction)
{
    current_traction_ = traction;
}

