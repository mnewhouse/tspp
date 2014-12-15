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

#ifndef SERVER_MESSAGES_HPP
#define SERVER_MESSAGES_HPP

#include "generic_client.hpp"

#include "messages/message.hpp"
#include "messages/message_center.hpp"

namespace ts
{
    namespace server
    {
        using messages::Message;

        enum class Message_type
        {
            Fast,
            Reliable
        };

        struct Client_message
        {
            Generic_client client;
            Message message;
            Message_type message_type = Message_type::Reliable;
        };

        using Message_center = messages::Message_center<Client_message>;
        using Message_listener = messages::Message_listener<Client_message>;
        using Message_dispatcher = messages::Message_dispatcher<Client_message>;

        using Scoped_message_listener = messages::Scoped_message_listener<Client_message>;
        using Scoped_message_dispatcher = messages::Scoped_message_dispatcher<Client_message>;
    }
}

#endif