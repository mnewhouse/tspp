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

#ifndef MESSAGE_LISTENER_HPP
#define MESSAGE_LISTENER_HPP

namespace ts
{
    namespace messages
    {
        template <typename MessageType>
        class Message_listener
        {
        public:
            using message_type = MessageType;

            virtual ~Message_listener() = default;
            virtual void handle_message(const MessageType& message) = 0;
        };

        template <typename MessageType>
        using Scoped_message_listener = core::Scoped_listener<Message_listener<MessageType>>;

        template <typename MessageType>
        class Message_listener_host
            : public core::Listener_host<Message_listener<MessageType>>
        {
        public:
            using listener_type = Message_listener<MessageType>;

            void handle_message(const MessageType& message) const
            {
                call_listeners(&listener_type::handle_message, message);
            }
        };
    }
}

#endif