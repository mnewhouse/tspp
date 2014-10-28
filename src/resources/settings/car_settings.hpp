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

#ifndef CAR_SETTINGS_HPP
#define CAR_SETTINGS_HPP

#include "resources/car_identifier.hpp"

namespace ts
{
    namespace resources
    {
        class Car_store;

        enum class Car_mode
        {
            Fixed,
            Free,
            Random
        };
        
        class Car_settings
        {
        public:
            void select_car(const Car_identifier& car);
            void deselect_car(const Car_identifier& car);

            const std::vector<Car_identifier>& selected_cars() const;

            void set_car_mode(Car_mode car_mode);
            Car_mode car_mode() const;

            bool is_car_selected(const Car_identifier& car) const;

        private:
            std::vector<Car_identifier> selected_cars_;
            Car_mode car_mode_ = Car_mode::Free;
        };
    }
}

#endif