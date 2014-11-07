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
#include "handling.hpp"
#include "car.hpp"
#include "resources/terrain_definition.hpp"

#include <SFML/Graphics.hpp>

std::istream& ts::world::operator>>(std::istream& stream, Handling_properties& properties)
{
    for (std::string line, directive; directive != "end" && std::getline(stream, line);)
    {
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;

        if (directive == "mass") 
        {
            line_stream >> properties.mass;
            properties.mass = std::max(1.0, properties.mass);
        }

        else if (directive == "dragfactor") 
        {
            line_stream >> properties.drag_factor;
        }

        else if (directive == "rollingresistance") 
        {
            line_stream >> properties.rolling_resistance;
        }

        else if (directive == "lateralfriction")
        {
            line_stream >> properties.lateral_friction;
        }

        else if (directive == "steering") 
        {
            line_stream >> properties.steering;
        }

        else if (directive == "steeringlock")
        {
            line_stream >> properties.steering_lock;
        }

        else if (directive == "turningresistance")
        {
            line_stream >> properties.turning_resistance;
        }

        else if (directive == "braking") 
        {
            line_stream >> properties.braking;
        }

        else if (directive == "power") 
        {
            if (line_stream >> properties.power)
            {
                double reverse;
                if (line_stream >> reverse)
                {
                    properties.reverse_power = reverse;
                }
            }
        }

        else if (directive == "grip") 
        {
            line_stream >> properties.grip;
        }

        else if (directive == "antislide") 
        {
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

        else if (directive == "tractionrecovery")
        {
            line_stream >> properties.traction_recovery;
        }

        else if (directive == "slidethreshold")
        {
            line_stream >> properties.slide_threshold;
        }

        else if (directive == "reversethreshold")
        {
            line_stream >> properties.reverse_threshold;
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
    auto reverse_rotation = rotation + Rotation<double>::degrees(180);

    auto heading = Rotation<double>::radians(std::atan2(velocity.x, -velocity.y));
    auto heading_vec = normalize(velocity);

    auto mass = properties.mass;
    auto speed = magnitude(velocity);

    auto oversteer = rotation - heading;
    auto reverse_oversteer = reverse_rotation - heading;

    Vector2<double> rotation_vec(std::sin(rotation.radians()), -std::cos(rotation.radians()));

    if (speed != 0.0) 
    {
        if (!car.is_reversing()) 
        {
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
    auto resistance = -velocity * 0.5 * speed * properties.drag_factor; // drag

    if (speed != 0.0) 
    {
        resistance += -heading_vec * properties.rolling_resistance * mass;

        Vector2<double> lateral_vec(-rotation_vec.y, rotation_vec.x);
        if (dot_product(lateral_vec, heading_vec) < 0.0)
        {
            lateral_vec.x = -lateral_vec.x; lateral_vec.y = -lateral_vec.y;
        }

        // lateral friction
        resistance += lateral_vec * dot_product(lateral_vec, -velocity) * properties.lateral_friction;

        resistance += -velocity * terrain.roughness;
    }

    const auto& traction_limit = properties.traction_limit;
    const auto& traction_loss = properties.traction_loss;
    double required_traction = 0.0;

    if (car.control_state(Control::Accelerate)) 
    {
        auto force_1d = properties.power * 1000.0 * terrain.acceleration;
        auto force = rotation_vec * force_1d;

        acceleration_force += force;
        required_traction += force_1d;
    }

    if (car.control_state(Control::Brake)) 
    {
        if (speed == 0.0 || car.is_reversing()) 
        {
            auto force_1d = properties.reverse_power * 1000.0 * terrain.acceleration;
            auto force = -rotation_vec * force_1d;

            acceleration_force += force;
            required_traction += force_1d;
        }

        else 
        {
            auto force_1d = properties.braking * 1000.0 * terrain.braking;
            auto force = -heading_vec * force_1d;
            braking_force += force;

            required_traction += force_1d * (traction_limit.accelerate / traction_limit.brake);   
        }
    }

    required_traction *= 0.001;
    
    double steering_multiplier = 0.0;
    if (car.control_state(Control::Left)) 
    {
        steering_multiplier -= 1.0;
    }

    if (car.control_state(Control::Right)) 
    {
        steering_multiplier += 1.0;
    }


    auto new_traction = terrain.traction, old_traction = car.current_traction();
    if (speed != 0.0) {
        new_traction *= std::abs(std::abs(oversteer.radians()) < std::abs(reverse_oversteer.radians()) ?
                                    std::cos(oversteer.radians()) : std::cos(reverse_oversteer.radians()));
    }

    auto required_lateral_traction = (steering_multiplier != 0.0 ? properties.steering * terrain.steering * speed : 0.0);

    auto total_required_traction = 0.0;
    if (traction_limit.accelerate != 0.0 && traction_limit.lateral != 0.0)
    {
        auto accel = required_traction / traction_limit.accelerate;
        auto lateral = required_lateral_traction / traction_limit.lateral;

        total_required_traction = std::sqrt(accel * accel + lateral * lateral);
    }

    if (total_required_traction >= 1.0)
    {
        new_traction /= total_required_traction;
        new_traction = std::max(1.0 - (1.0 - new_traction) * traction_loss.multiplier, 0.0);
    }


    auto current_traction = new_traction;
    if (speed == 0.0)
    {
        current_traction = 1.0;
    }

    else if (new_traction >= old_traction)
    {
        if (properties.traction_recovery == 0.0 || total_required_traction == 0.0)
        {
            current_traction = new_traction;
        }
        
        else
        {
            auto traction_recovery = properties.traction_recovery / std::max(total_required_traction, 0.5);
            current_traction = std::min(old_traction + (frame_duration * traction_recovery), new_traction);
        }
    }    

    acceleration_force *= 1.0 - (1.0 - current_traction) * (1.0 - traction_loss.accelerate_effect);
    braking_force *= 1.0 - (1.0 - current_traction) * (1.0 - traction_loss.brake_effect); 

    auto antislide = (properties.antislide * terrain.antislide);
    antislide *= 1.0 - (1.0 - current_traction) * (1.0 - traction_loss.antislide_effect);

    auto steering = properties.steering;
    auto steering_lock = properties.steering_lock;

    if (speed != 0.0)
    {
        auto grip = properties.grip;

        // If grip / speed < steering_lock, use the former value instead of steering_lock
        auto grip_steering = std::min(steering_lock, grip / speed);

        if (grip != 0.0)
        {
            // If antislide > 0, reduce steering so that it actually does antislide.
            steering_lock -= std::min(antislide / grip, 1.0) * (steering_lock - grip_steering);        
        }
    }

    auto steering_effect = 1.0 - (1.0 - current_traction) * (1.0 - traction_loss.steering_effect);
    steering *= terrain.steering;
    steering *= steering_effect;

    steering_lock *= terrain.steering;
    steering_lock *= steering_effect;

    auto turning_speed = car.angular_velocity();
    auto target_turning_speed = steering_multiplier * steering_lock;

    auto new_turning_speed = turning_speed;
    if (properties.steering == 0.0 && properties.steering_lock != 0.0)
    {
        turning_speed = target_turning_speed;
    }

    else
    {
        // Turning resistance moves the turning speed towards zero,
        // Steering moves the turning speed towards the target turning speed, IF steering is active.
        auto turning_resistance = properties.turning_resistance * frame_duration;
        turning_resistance += turning_resistance * terrain.roughness;
        if (turning_speed >= 0.0) turning_resistance = -turning_resistance;

        auto new_turning_speed = turning_speed;
        if (turning_speed < target_turning_speed)
        {
            new_turning_speed = std::min(turning_speed + steering + turning_resistance, target_turning_speed);
        }

        else if (turning_speed > target_turning_speed)
        {
            new_turning_speed = std::max(turning_speed - steering + turning_resistance, target_turning_speed);
        }

        if (target_turning_speed < 0.0 && turning_speed < 0.0)
        {
            new_turning_speed = std::min(new_turning_speed, 0.0);
        }

        else if (target_turning_speed > 0.0 && turning_speed > 0.0)
        {
            new_turning_speed = std::max(new_turning_speed, 0.0);
        }

        turning_speed = new_turning_speed;
    }
    

    auto net_force = acceleration_force + braking_force + resistance;    
    velocity += (net_force * frame_duration) / mass;

    {
        auto new_heading = normalize(velocity);
        if (dot_product(new_heading, heading_vec) < 0.0) velocity = {};

        heading_vec = new_heading;
    }
    
    auto new_speed = magnitude(velocity);
    if (new_speed != 0.0) 
    {
        auto new_heading = Rotation<double>::radians(std::atan2(velocity.x, -velocity.y));
        auto new_oversteer = rotation - new_heading;
        auto new_reverse_oversteer = reverse_rotation - new_heading;

        auto max_correction = std::abs(new_reverse_oversteer.radians()) < std::abs(new_oversteer.radians()) ?
            new_reverse_oversteer : new_oversteer;

        auto correction = std::min((antislide / new_speed) * frame_duration, std::abs(max_correction.radians()));
        if (max_correction.radians() < 0.0) correction = -correction;

        new_heading += Rotation<double>::radians(correction);
        Vector2<double> new_heading_vec(std::sin(new_heading.radians()), -std::cos(new_heading.radians()));

        velocity = new_heading_vec * new_speed;
    }

    car.set_current_traction(current_traction);
    car.set_angular_velocity(turning_speed);
    car.set_velocity(velocity);
}

double ts::world::Handling::top_speed(const Handling_properties& properties)
{
    if (properties.drag_factor == 0.0) return 0;
   
    auto squared = (properties.power - properties.rolling_resistance) * 1000.0 / (0.5 * properties.drag_factor);
    return squared > 0.0 ? std::sqrt(squared) : 0.0;
}