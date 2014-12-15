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
#include "chatbox_interface.hpp"

#include "cup/cup_messages.hpp"

ts::client::Chatbox_interface::Chatbox_interface(Message_center* message_center)
: Scoped_message_listener(message_center)
{
}

void ts::client::Chatbox_interface::handle_message(const Server_message& server_message)
{
    using Msg = cup::Message_type;

    auto message_type = server_message.message_type;
    const auto& message = server_message.message;

    if (message_type == Message_type::Reliable && message.type() == Msg::chatbox_output)
    {
        handle_chatbox_output(message);
    }
}

void ts::client::Chatbox_interface::handle_chatbox_output(const Message& message)
{
    auto chatbox_output = cup::parse_chatbox_output_message(message);

    chatbox_.dispatch_message(chatbox_output.message);
}

const ts::cup::Chatbox* ts::client::Chatbox_interface::chatbox() const
{
    return &chatbox_;
}

ts::cup::Chatbox* ts::client::Chatbox_interface::chatbox()
{
    return &chatbox_;
}

ts::core::Listener_host<ts::cup::Chatbox_listener>* ts::client::Chatbox_interface::chatbox_listener_host()
{
    return &chatbox_;
}