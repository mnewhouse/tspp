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
#include "car_settings.hpp"

#include "resources/car_store.hpp"

const std::vector<ts::resources::Car_handle>& ts::resources::Car_settings::selected_cars() const
{
    return selected_cars_;
}

ts::resources::Car_mode ts::resources::Car_settings::car_mode() const
{
    return car_mode_;
}

void ts::resources::Car_settings::select_car(Car_handle car_handle)
{
    if (car_mode_ == Car_mode::Fixed)
    {
        selected_cars_.clear();
        selected_cars_.push_back(car_handle);
    }

    else if (!is_car_selected(car_handle))
    {
        selected_cars_.push_back(car_handle);
    }
}

void ts::resources::Car_settings::deselect_car(Car_handle car_handle)
{
    if (selected_cars_.size() != 1 || selected_cars_.front() != car_handle)
    {
        auto remove_it = std::remove(selected_cars_.begin(), selected_cars_.end(), car_handle);
        selected_cars_.erase(remove_it, selected_cars_.end());
    }
}

void ts::resources::Car_settings::set_car_mode(Car_mode car_mode)
{
    if (car_mode == Car_mode::Fixed)
    {
        if (!selected_cars_.empty()) selected_cars_.resize(1);
    }

    car_mode_ = car_mode;
}

bool ts::resources::Car_settings::is_car_selected(Car_handle car_handle) const
{
    return std::find(selected_cars_.begin(), selected_cars_.end(), car_handle) != selected_cars_.end();
}


void ts::resources::Car_settings::load_car_definitions(const Car_store* car_store)
{
    for (auto& car : selected_cars_)
    {
        car = car_store->get_car_by_name(car.car_name());
    }
}