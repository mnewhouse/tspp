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

namespace ts
{
    namespace world
    {
        class World;
        class Car;
    }

    namespace controls
    {
        class Controllable;
    }

    namespace action
    {
        using messages::Message;

        struct Message_type
        {
            static const std::uint32_t game_state = 5111;
        };

        struct Car_state
        {
            std::uint16_t car_id;
            Vector2<double> position;
            Vector2<double> velocity;            
            Rotation<double> rotation;
            double z_position;            
        };

        struct Game_state_message
        {
            std::uint32_t message_type = 0;
            std::vector<Car_state> car_states;
        };

        Message make_game_state_message(const world::World* world);

        Message make_control_event_message(const controls::Controllable* controllable, std::uint32_t stage_time);
        bool is_control_event_message(const Message& message);

    }
}

#endif

