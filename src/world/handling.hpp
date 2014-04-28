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

        struct Traction_limit
        {
            double accelerate;
            double brake;
            double lateral;
        };

        struct Traction_loss
        {
            double multiplier;
            double steering_effect;
            double antislide_effect;
            double accelerate_effect;
            double brake_effect;
        };

        struct Handling_properties
        {
            Handling_properties();

            double mass;

            double power;
            double reverse_power;

            double braking;
            
            double grip;
            double steering;
            double antislide;
           
            double drag_coefficient;
            double roll_resistance;
            double lateral_friction;

            Traction_limit traction_limit;
            Traction_loss traction_loss;           

            double reverse_threshold;
            double slide_threshold;
        };

        struct Handling
        {
            void update(const Handling_properties& properties, Car& car, double frame_duration);

            static double top_speed(const Handling_properties& properties);

        };

        std::istream& operator>>(std::istream& stream, Handling_properties& properties);

    }
}

#endif