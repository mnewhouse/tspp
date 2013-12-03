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

#ifndef WORLD_CAR_HANDLING_HPP
#define WORLD_CAR_HANDLING_HPP

#include <istream>

namespace ts
{
    namespace resources
    {
        struct Terrain_definition;
    }

    namespace world
    {
        class Car;

        struct Handling_properties
        {
            Handling_properties();

            double mass;

            double power;
            double reverse_power;
            
            double steering;
            double grip;
            double grip_reduction;
            double anti_slide;
           
            double drag_coefficient;
            double friction;

            double braking;
            double braking_grip;         
            

            double non_slide_sector;
            double slide_braking;
            double slide_deceleration;
        };

        struct Handling
        {
            void update(const Handling_properties& properties, Car& car, double frame_duration);
        };

        std::istream& operator>>(std::istream& stream, Handling_properties& properties);

    }
}

#endif