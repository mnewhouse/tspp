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
#include "action_messages.hpp"
#include "stage.hpp"

#include "messages/message_reader.hpp"

#include "world/world.hpp"
#include "world/car.hpp"

namespace ts
{
    namespace action
    {
        using messages::Message_reader;

        Message_reader& operator>>(Message_reader& message_reader, Car_state& car_state);
    }
}

ts::action::Message ts::action::make_game_state_message(const action::Stage* stage)
{
    Message message(Message_type::game_state);
    std::uint32_t stage_time = stage->stage_time();

    Vector2<double> world_size = stage->world_size();
    const auto max_entity_speed = stage->world().max_entity_speed();

    const double position_max = (1 << 24) - 1;
    const double velocity_max = (1 << 23) - 1;



    const Vector2<double> world_size_multiplier(position_max / world_size.x, position_max / world_size.y);
    const double velocity_multiplier = velocity_max / max_entity_speed;
    const double rotation_multiplier = 32727.0 / 3.1415926535;
    const double traction_multiplier = 32727.0;

    const auto& car_data = stage->car_data();

    message << stage_time;
    message << static_cast<std::uint32_t>(world_size.x);
    message << static_cast<std::uint32_t>(world_size.y);
    message << static_cast<std::uint32_t>(max_entity_speed);
    message << static_cast<std::uint32_t>(car_data.size());

    using messages::uint24_t;
    using messages::int24_t;

    for (const auto& car_info : car_data)
    {
        const auto* car = car_info.car;
        const auto& position = car->position();
        const auto& velocity = car->velocity();
        const auto& rotation = car->rotation();
        double angular_velocity = car->angular_velocity();
        double traction = car->current_traction();
        bool reversing = car->is_reversing();

        message << static_cast<std::uint16_t>(car_info.car_id);

        auto x_pos = static_cast<std::uint32_t>(position.x * world_size_multiplier.x);
        auto y_pos = static_cast<std::uint32_t>(position.y * world_size_multiplier.y);
        message << uint24_t(x_pos);
        message << uint24_t(y_pos);
        message << std::uint16_t(0);

        auto x_velocity = static_cast<std::uint32_t>(velocity.x * velocity_multiplier);
        auto y_velocity = static_cast<std::uint32_t>(velocity.y * velocity_multiplier);
        message << int24_t(x_velocity);
        message << int24_t(y_velocity);

        message << std::int16_t(rotation.radians() * rotation_multiplier); // Rotation
        message << int24_t(angular_velocity * velocity_multiplier);

        std::uint16_t traction_data = std::uint16_t(traction * traction_multiplier) & 0x7FFF;
        traction_data |= std::uint16_t(reversing << 15);

        message << traction_data;
        message << car->control_state_mask();

    }

    return message;
}

ts::action::Game_state_message ts::action::parse_game_state_message(const Message& message)
{
    messages::Message_reader message_reader(message);

    Game_state_message result;

    using messages::uint24_t;
    using messages::int24_t;

    std::uint32_t max_entity_speed, world_width, world_height, car_count;
    if (message_reader >> result.message_type >> result.stage_time >> world_width >> world_height >> max_entity_speed >> car_count)
    {
        const double position_max = (1 << 24) - 1;
        const double velocity_max = (1 << 23) - 1;

        const Vector2<double> world_size_multiplier(world_width / position_max, world_height / position_max);
        const double velocity_multiplier = max_entity_speed / velocity_max;
        const double rotation_multiplier = 3.1415926535 / 32727.0;
        const double traction_multiplier = 1.0 / 32727.0;

        std::uint16_t car_id;
        uint24_t x_pos, y_pos;
        std::uint16_t z_pos;
        int24_t x_vel, y_vel;
        std::int16_t rotation;
        int24_t ang_vel;
        std::uint16_t traction;
        std::uint16_t controls_mask;

        for (std::uint32_t n = 0; n != car_count && message_reader >> car_id >> x_pos >> y_pos >> z_pos >> 
             x_vel >> y_vel >> rotation >> ang_vel >> traction >> controls_mask; ++n)
        {
            Car_state car_state;
            car_state.car_id = car_id;

            car_state.position.x = x_pos.value * world_size_multiplier.x;
            car_state.position.y = y_pos.value * world_size_multiplier.y;
            car_state.z_position = 0.0;

            car_state.velocity.x = x_vel.value * velocity_multiplier;
            car_state.velocity.y = y_vel.value * velocity_multiplier;

            car_state.rotation = Rotation<double>::radians(rotation * rotation_multiplier);
            car_state.angular_velocity = ang_vel.value * velocity_multiplier;

            car_state.reversing = (traction & 0x8000) != 0;
            car_state.traction = (traction & 0x7FFF) * traction_multiplier;

            car_state.controls_mask = controls_mask;
            result.car_states.push_back(car_state);
        }
    }
    
    return result;
}

ts::action::Message ts::action::make_control_event_message(std::uint16_t controllable_id, std::uint16_t controls_mask,
                                                           const std::uint32_t stage_time)
{
    Message result;

    controls_mask |= 0x8000;
    result << controls_mask;
    result << controllable_id;
    result << stage_time;
    return result;
}

bool ts::action::is_control_event_message(const Message& message)
{
    return !message.empty() && (*message.data() & 0x80) != 0;
}

ts::action::Control_event_message ts::action::parse_control_event_message(const Message& message)
{
    Message_reader message_reader(message);

    Control_event_message result;
    std::uint16_t state_data = 0;
    std::uint16_t controllable_id = 0;
    std::uint32_t stage_time = 0;
    if (message_reader >> state_data >> controllable_id >> stage_time)
    {
        result.controls_mask = state_data & 0x7FFF;
        result.controllable_id = controllable_id;
        result.stage_time = stage_time;
    }

    return result;
}