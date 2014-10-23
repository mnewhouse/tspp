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
#include "server_stage_conductor.hpp"

#include "action/action_messages.hpp"
#include "action/stage_conductor.hpp"

ts::server::Stage_conductor::Stage_conductor(Message_center* message_center, action::Stage* stage)
: Message_listener(message_center),
  stage_conductor_(std::make_unique<action::Stage_conductor>(stage)),
  message_center_(message_center)
{
}

void ts::server::Stage_conductor::update(std::size_t frame_duration)
{
    stage_conductor_->update(frame_duration);

    if (++update_count_ == 5)
    {
        update_count_ = 0;
        message_buffer_.client = Generic_client();
        message_buffer_.message = action::make_game_state_message(stage_conductor_->stage());        
        message_buffer_.message_type = Message_type::Fast;

        message_center_->dispatch_message(message_buffer_);
    }
}

void ts::server::Stage_conductor::handle_message(const Client_message& client_message)
{
    const auto& message = client_message.message;
    const auto& client = client_message.client;

    if (client_message.message_type == Message_type::Fast && action::is_control_event_message(message))
    {
        handle_control_event_message(message);
    }
}

void ts::server::Stage_conductor::handle_control_event_message(const Message& message)
{
    auto event = action::parse_control_event_message(message);
    stage_conductor_->process_control_event_message(event);

    // Send the event we got from the client to the other clients.
    message_buffer_.message = message;
    message_buffer_.client = Generic_client();
    message_buffer_.message_type = Message_type::Fast;
    message_center_->dispatch_message(message_buffer_);
}