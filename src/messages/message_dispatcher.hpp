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

#ifndef MESSAGE_DISPATCHER_HPP
#define MESSAGE_DISPATCHER_HPP

namespace ts
{
    namespace messages
    {
        template <typename MessageType>
        class Message_dispatcher
        {
        public:
            virtual ~Message_dispatcher() = default;
            virtual void dispatch_message(const MessageType& message) = 0;            
        };

        template <typename MessageType>
        using Scoped_message_dispatcher = core::Scoped_listener<Message_dispatcher<MessageType>>;

        template <typename MessageType>
        class Message_dispatcher_host
            : public core::Listener_host<Message_dispatcher<MessageType>>
        {
        public:
            using dispatcher_type = Message_dispatcher<MessageType>;

            void dispatch_message(const MessageType& message) const
            {
                call_listeners(&dispatcher_type::dispatch_message, message);
            }
        };
    }
}

#endif