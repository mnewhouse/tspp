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

#include "handling.hpp"
#include "car.hpp"

#include "core/directive_reader.hpp"
#include "core/rotation.hpp"

#include "resources/terrain_definition.hpp"

#include <SFML/Graphics.hpp>
#include <sstream>

ts::world::Handling_properties::Handling_properties()
: mass(1.0),

  power(0.0),
  reverse_power(0.0),

  braking(0.0),

  steering(0.0),
  grip(0.0),
  antislide(0.0),

  drag_coefficient(0.0),
  roll_resistance(0.0),
  lateral_friction(0.0),

  traction_limit({ 0.0, 0.0, 0.0 }),
  traction_loss({ 0.0, 0.0, 0.0, 0.0, 0.0 }),

  reverse_threshold(0.1)
{
}

std::istream& ts::world::operator>>(std::istream& stream, Handling_properties& properties)
{
    for (std::string line, directive; directive != "end" && std::getline(stream, line);)
    {
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;

        if (directive == "mass") {
            line_stream >> properties.mass;
            properties.mass = std::max(1.0, properties.mass);
        }

        else if (directive == "dragcoefficient") {
            line_stream >> properties.drag_coefficient;
        }

        else if (directive == "rollresistance") {
            line_stream >> properties.roll_resistance;
        }

        else if (directive == "lateralfriction")
        {
            line_stream >> properties.lateral_friction;
        }

        else if (directive == "steering") {
            line_stream >> properties.steering;
        }

        else if (directive == "braking") {
            line_stream >> properties.braking;
        }

        else if (directive == "power") {
            if (line_stream >> properties.power)
            {
                double reverse;
                if (line_stream >> reverse)
                {
                    properties.reverse_power = reverse;
                }
            }
        }

        else if (directive == "grip") {
            line_stream >> properties.grip;
        }

        else if (directive == "antislide") {
            line_stream >> properties.antislide;
        }

        else if (directive == "tractionlimit")
        {
            double accelerate, brake, lateral;
            if (line_stream >> accelerate >> brake >> lateral)
            {
                properties.traction_limit.accelerate = accelerate;
                properties.traction_limit.brake = brake;
                properties.traction_limit.lateral = lateral;
            }
        }

        else if (directive == "tractionloss")
        {
            double multiplier, accelerate, brake, steering, antislide;
            if (line_stream >> multiplier >> accelerate >> brake >> steering >> antislide)
            {
                properties.traction_loss.multiplier = multiplier;
                properties.traction_loss.accelerate_effect = accelerate;
                properties.traction_loss.brake_effect = brake;
                properties.traction_loss.steering_effect = steering;
                properties.traction_loss.antislide_effect = antislide;
            }
        }
    }

    return stream;
}

void ts::world::Handling::update(const Handling_properties& properties, Car& car, double frame_duration)
{
    using controls::Control;

    const auto& terrain = car.current_terrain();

    auto velocity = car.velocity();

    auto rotation = car.rotation();
    auto reverse_rotation = -rotation;

    auto heading = Rotation<double>::radians(std::atan2(velocity.x, -velocity.y));
    auto heading_vec = normalize(velocity);

    auto mass = properties.mass;
    auto speed = magnitude(velocity);

    auto oversteer = rotation - heading;
    auto reverse_oversteer = reverse_rotation - heading;

    auto grip = properties.grip;
    auto steering = std::min(properties.steering, properties.grip / speed);

    Vector2<double> rotation_vec(std::sin(rotation.radians()), -std::cos(rotation.radians()));

    if (speed != 0.0) {
        if (!car.is_reversing()) {
            if (!car.control_state(Control::Accelerate) &&
                std::abs(reverse_oversteer.radians()) < properties.reverse_threshold)
            {
                car.toggle_reversing(true);
            }                
        }

        else if (!car.control_state(Control::Brake) &&
                 std::abs(oversteer.radians()) < properties.reverse_threshold)
        {
            car.toggle_reversing(false);                
        }
    }

    Vector2<double> acceleration_force, braking_force;
    auto resistance = -heading_vec * 0.5 * speed * speed * properties.drag_coefficient; // drag

    if (speed != 0.0) {
        resistance += -heading_vec * properties.roll_resistance * 1000.0;

        Vector2<double> lateral_vec(-rotation_vec.y, rotation_vec.x);
        if (dot_product(lateral_vec, heading_vec) < 0.0)
        {
            lateral_vec.x = -lateral_vec.x; lateral_vec.y = -lateral_vec.y;
        }

        // lateral friction
        resistance += lateral_vec * dot_product(lateral_vec, -heading_vec) * properties.lateral_friction * 1000.0;
    }

    const auto& traction_limit = properties.traction_limit;
    const auto& traction_loss = properties.traction_loss;
    double required_traction = 0.0;

    if (car.control_state(Control::Accelerate)) {
        auto force = rotation_vec * properties.power * 1000.0;
        acceleration_force += force;
        required_traction += magnitude(force + resistance);
    }

    if (car.control_state(Control::Brake)) {
        if (speed == 0.0 || car.is_reversing()) {
            auto force = -rotation_vec * properties.reverse_power * 1000.0;
            acceleration_force += force;
            required_traction += magnitude(force + resistance);
        }

        else {
            auto force = -heading_vec * properties.braking * 1000.0;
            braking_force += force;
            required_traction += magnitude(force + resistance) * (traction_limit.accelerate / traction_limit.brake);
        }
    }

    required_traction *= 0.001;
    
    double steering_speed = 0.0;
    if (car.control_state(Control::Left)) {
        steering_speed -= steering;
    }

    if (car.control_state(Control::Right)) {
        steering_speed += steering;
    }


    auto current_traction = 1.0;
    if (speed != 0.0) {
    //    current_traction = std::abs(std::abs(oversteer.radians()) < std::abs(reverse_oversteer.radians()) ?
    //                                std::cos(oversteer.radians()) : std::cos(reverse_oversteer.radians()));
    }

    auto required_lateral_traction = (steering_speed != 0.0 ? properties.steering * speed : 0.0);

    auto total_required_traction = std::pow(required_traction / traction_limit.accelerate, 2) +
        std::pow(required_lateral_traction / traction_limit.lateral, 2);

    if (total_required_traction >= 1.0)
    {
        total_required_traction = std::sqrt(total_required_traction);
        current_traction /= total_required_traction * std::max(traction_loss.multiplier, 1.0);
    }

    acceleration_force -= acceleration_force * (1.0 - current_traction) * traction_loss.accelerate_effect;
    braking_force -= braking_force * (1.0 - current_traction) * traction_loss.brake_effect;
    steering_speed -= steering_speed * (1.0 - current_traction) * traction_loss.steering_effect;

    auto net_force = acceleration_force + braking_force + resistance;
    
    velocity += (net_force * frame_duration) / mass;

    {
        auto new_heading = normalize(velocity);
        if (dot_product(new_heading, heading_vec) < 0.0) velocity = {};

        heading_vec = new_heading;
    }
    
    if (speed != 0.0) {
        auto new_heading = Rotation<double>::radians(std::atan2(velocity.x, -velocity.y));
        auto new_oversteer = rotation - new_heading;
        auto new_reverse_oversteer = reverse_rotation - new_heading;
        auto new_speed = magnitude(velocity);

        auto max_correction = std::abs(new_reverse_oversteer.radians()) < std::abs(new_oversteer.radians()) ?
            new_reverse_oversteer : new_oversteer;


        auto antislide = (properties.antislide / new_speed);
        antislide -= antislide * (1.0 - current_traction) * traction_loss.antislide_effect;

        auto correction = std::min((properties.antislide / new_speed) * frame_duration, std::abs(max_correction.radians()));
        if (max_correction.radians() < 0.0) correction = -correction;

        new_heading += Rotation<double>::radians(correction);
        Vector2<double> new_heading_vec(std::sin(new_heading.radians()), -std::cos(new_heading.radians()));

        velocity = new_heading_vec * new_speed;
    }

    car.set_current_traction(current_traction);
    car.set_angular_velocity(steering_speed);
    car.set_velocity(velocity);
}