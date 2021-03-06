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
#include "car_handle.hpp"

ts::resources::Car_handle::Car_handle(const Car_definition* car_def)
: Pointer_handle(car_def)
{
    if (car_def)
    {
        car_name_ = car_def->car_name;
    }
}

ts::resources::Car_handle::Car_handle(utf8_string car_name)
: car_name_(car_name)
{
}

const ts::utf8_string& ts::resources::Car_handle::car_name() const
{
    return car_name_;
}