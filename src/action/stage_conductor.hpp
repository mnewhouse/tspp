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

#ifndef ACTION_STAGE_CONDUCTOR_HPP
#define ACTION_STAGE_CONDUCTOR_HPP

#include "action_messages.hpp"

namespace ts
{
    namespace action
    {
        class Stage;

        class Stage_conductor
        {
        public:
            Stage_conductor(Stage* stage);
            ~Stage_conductor();

            void process_control_event_message(const Control_event_message& message);
            void process_game_state_message(const Game_state_message& message);

            void update(std::size_t frame_duration);

            const Stage* stage() const;            
            std::uint32_t stage_time() const;
            void purge_old_events(std::uint32_t stage_time_threshold);

        private:
            void handle_control_event_message(const Control_event_message& message);
            void enqueue_control_event_message(const Control_event_message& message);            

            void poll();

            std::deque<Control_event_message> control_event_queue_;
            Stage* stage_;
        };
    }
}

#endif