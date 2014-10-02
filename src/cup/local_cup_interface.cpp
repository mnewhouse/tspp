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
#include "local_cup_interface.hpp"

ts::cup::Local_cup_interface::Local_cup_interface(Cup* cup)
: Cup_interface(cup)
{
}


void ts::cup::Local_cup_interface::select_cars(const std::vector<Car_selection>& car_selection)
{
    for (auto& selected_car : car_selection)
    {
        set_player_car(selected_car.player_handle, selected_car.car_handle);
    }

    advance();
}

void ts::cup::Local_cup_interface::signal_ready()
{
    advance();
}