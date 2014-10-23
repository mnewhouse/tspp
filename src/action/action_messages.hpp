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

#ifndef ACTION_MESSAGES_HPP
#define ACTION_MESSAGES_HPP

#include "messages/message.hpp"

#include "controls/control.hpp"

namespace ts
{
    namespace world
    {
        class Car;
    }

    namespace action
    {
        class Stage;
        using messages::Message;

        struct Message_type
        {
            static const std::uint32_t game_state = 5111;
        };

        struct Car_state
        {
            std::uint16_t car_id;
            Vector2<double> position;
            double z_position;

            Vector2<double> velocity;
            Rotation<double> rotation;
            double angular_velocity;
            
            bool reversing;
            double traction;

            std::uint16_t controls_mask;
        };

        struct Game_state_message
        {
            std::uint32_t message_type = 0;
            std::uint32_t stage_time = 0;
            std::vector<Car_state> car_states;
        };

        struct Control_event_message
        {
            std::uint16_t controllable_id = 0;
            std::uint16_t controls_mask = 0;
            std::uint32_t stage_time = 0;            
        };

        Message make_game_state_message(const action::Stage* stage);
        Game_state_message parse_game_state_message(const Message& message);

        Message make_control_event_message(std::uint16_t controllable_id, std::uint16_t controls_mask, std::uint32_t stage_time);
        bool is_control_event_message(const Message& message);
        Control_event_message parse_control_event_message(const Message& message);

    }
}

#endif

