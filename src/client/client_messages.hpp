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

#ifndef CLIENT_MESSAGES_HPP
#define CLIENT_MESSAGES_HPP

#include "messages/message_center.hpp"
#include "messages/message.hpp"

namespace ts
{
    namespace client
    {
        using messages::Message;

        enum class Message_type
        {
            Fast,
            Reliable
        };        

        struct Server_message
        {
            Message message;
            Message_type message_type = Message_type::Reliable;
        };

        using Message_center = messages::Message_center<Server_message>;
        using Message_listener = messages::Message_listener<Server_message>;
        using Message_dispatcher = messages::Message_dispatcher<Server_message>;

        using Scoped_message_listener = messages::Scoped_message_listener<Server_message>;
        using Scoped_message_dispatcher = messages::Scoped_message_dispatcher<Server_message>;
    }
}

#endif