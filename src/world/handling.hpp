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
            double accelerate = 0.0;
            double brake = 0.0;
            double lateral = 0.0;
        };

        struct Traction_loss
        {
            double multiplier = 0.0;
            double steering_effect = 0.0;
            double antislide_effect = 0.0;
            double accelerate_effect = 0.0;
            double brake_effect = 0.0;
        };

        struct Handling_properties
        {
            double mass = 100.0;

            double power = 0.0;
            double reverse_power = 0.0;

            double braking = 0.0;
            
            double grip = 0.0;
            double steering = 0.0;
            double steering_lock = 0.0;
            double antislide = 0.0;
           
            double drag_factor = 1.0;
            double rolling_resistance = 0.0;
            double lateral_friction = 0.0;
            double turning_resistance = 0.0;

            Traction_limit traction_limit;
            Traction_loss traction_loss;
            double traction_recovery = 1.0;

            double reverse_threshold = 0.1;
            double slide_threshold = 0.9;
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