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
: power(0.0),
  reverse_power(0.0),
  mass(0.0),
  grip(0.0),
  steering(0.0),
  grip_reduction(0.0),
  anti_slide(0.0),
  drag_coefficient(0.0),
  friction(0.0),  
  braking(0.0),
  braking_grip(0.0),
  non_slide_sector(0.0),
  slide_braking(0.0),
  slide_deceleration(0.0)
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
        }

        else if (directive == "dragcoefficient") {
            line_stream >> properties.drag_coefficient;
        }

        else if (directive == "friction") {
            line_stream >> properties.friction;
        }

        else if (directive == "steering") {
            line_stream >> properties.steering;
        }

        else if (directive == "braking") {
            line_stream >> properties.braking;
        }

        else if (directive == "power") {
            line_stream >> properties.power;
        }

        else if (directive == "reversepower") {
            line_stream >> properties.reverse_power;
        }

        else if (directive == "grip") {
            line_stream >> properties.grip;
        }

        else if (directive == "gripreduction") {
            line_stream >> properties.grip_reduction;
        }

        else if (directive == "antislide") {
            line_stream >> properties.anti_slide;
        }

        else if (directive == "brakinggrip") {
            line_stream >> properties.braking_grip;
        }

        else if (directive == "slidebraking") {
            line_stream >> properties.slide_braking;
        }

        else if (directive == "slidedeceleration") {
            line_stream >> properties.slide_deceleration;
        }

        else if (directive == "nonslidesector") {
            line_stream >> properties.non_slide_sector;
        }
    }

    return stream;
}

void ts::world::Handling::update(const Handling_properties& properties, Car& car, double frame_duration)
{
    static const double pi = 3.14159265358979323846;

    using controls::Control;

    const auto& terrain = car.current_terrain();

    auto angular_velocity = car.angular_velocity();
    auto velocity = car.velocity();
    auto rotation = car.rotation();
    auto heading = Rotation<double>::radians(std::atan2(velocity.x, -velocity.y));

    auto mass = properties.mass;
    auto speed = magnitude(velocity);

    auto oversteer = rotation - heading;

    auto grip = properties.grip - properties.grip_reduction * (speed / 100.0);
    grip *= std::abs(terrain.grip);

    auto steering = properties.steering;
    steering -= std::max((steering - grip) * properties.anti_slide * std::abs(terrain.anti_slide), 0.0);
    steering *= std::abs(terrain.steering);

    double steering_speed = 0.0;
    double braking = 0.0;
    double delta_speed = 0.0;

    sf::Transformable transformable;
    transformable.setRotation(float(rotation.degrees()));

    auto heading_vec = normalize(velocity);
    auto rotation_vec = transformable.getTransform().transformPoint(0.0, -1.0);
    auto heading_product = heading_vec.x * rotation_vec.x + heading_vec.y * rotation_vec.y;

    auto is_reversing = heading_product < -0.8;

    if (car.control_state(Control::Accelerate)) {
        delta_speed += std::abs(properties.power * 1000.0 / mass) * frame_duration * std::abs(terrain.acceleration);
    }

    if (car.control_state(Control::Brake)) {
        if (speed == 0.0 || is_reversing) {
            delta_speed -= std::abs(properties.reverse_power) * frame_duration * std::abs(terrain.acceleration);
        }

        else {
            braking += std::abs(properties.braking) * frame_duration * terrain.braking;
            grip *= properties.braking_grip;
        }
    }

    if (car.control_state(Control::Left)) {
        steering_speed -= steering;
    }

    if (car.control_state(Control::Right)) {
        steering_speed += steering;
    }

    bool is_sliding = speed != 0.0 && std::abs(oversteer.radians()) > properties.non_slide_sector;

    if (is_sliding) {
        braking += std::abs(properties.slide_braking) * frame_duration;

        if (delta_speed < 0.0) {
            delta_speed += std::abs(properties.slide_deceleration) * frame_duration;
            delta_speed = std::min(delta_speed, 0.0);
        }

        else {
            delta_speed -= std::abs(properties.slide_deceleration) * frame_duration;
            delta_speed = std::max(delta_speed, 0.0);
        }
    }


    if (braking > 0.0 && heading_product > 0.0)
    {
        // Braking effect
        braking *= heading_product;
        velocity -= heading_vec * braking;
    }

    {
        auto delta_velocity = transformable.getTransform().transformPoint(0.0f, float(-delta_speed));
        velocity.x += delta_velocity.x;
        velocity.y += delta_velocity.y;
        
        auto drag_force = speed * speed * properties.drag_coefficient;
        auto friction_force = speed * properties.friction;

        auto new_speed = magnitude(velocity);
        new_speed -= (drag_force / mass) * frame_duration;
        new_speed -= (friction_force) * frame_duration;

        velocity = normalize(velocity) * std::max(new_speed, 0.0);
    }

    if (grip != 0.0)
    {
        // Adjust velocity according to grip
        double adjustment = std::min(grip * frame_duration, std::abs(oversteer.radians()));
        if (oversteer.radians() < 0.0) adjustment = -adjustment;

        sf::Transformable grip_transform;
        grip_transform.setRotation(float(Rotation<double>::radians(adjustment).degrees()));

        auto adjusted_velocity = grip_transform.getTransform().transformPoint(float(velocity.x), 
                                                                              float(velocity.y));

        velocity.x = adjusted_velocity.x;
        velocity.y = adjusted_velocity.y;
    }


    car.set_angular_velocity(steering_speed);
    car.set_velocity(velocity);
}