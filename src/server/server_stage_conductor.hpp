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

#ifndef SERVER_STAGE_CONDUCTOR_HPP
#define SERVER_STAGE_CONDUCTOR_HPP

#include "server_messages.hpp"

namespace ts
{
    namespace action
    {
        class Stage;
        class Stage_conductor;
    }

    namespace server
    {
        class Stage_conductor
            : public Message_listener
        {
        public:
            Stage_conductor(Message_center* message_center, action::Stage* stage);

            void update(std::size_t frame_duration);

        private:
            virtual void handle_message(const Client_message& message) override;
            void handle_control_event_message(const Message& message);

            std::unique_ptr<action::Stage_conductor> stage_conductor_;
            std::size_t update_count_ = 0;
            Message_center* message_center_;
            Client_message message_buffer_;
        };
    }
}

#endif