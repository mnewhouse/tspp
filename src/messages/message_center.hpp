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

#ifndef MESSAGE_CENTER_HPP
#define MESSAGE_CENTER_HPP

#include "message_listener.hpp"
#include "message_dispatcher.hpp"

namespace ts
{
    namespace messages
    {
        template <typename InMessageType, typename OutMessageType>
        class Message_center
            : public Listener_set<InMessageType>, public Dispatcher_set<OutMessageType>
        {
        public:
            template <typename MessageSource>
            Message_center(MessageSource message_source);

            void handle_messages() const;

        private:
            std::function<bool(InMessageType&)> message_source_;

            mutable InMessageType message_buffer_;
        };
    }
}

template <typename InMessageType, typename OutMessageType>
template <typename MessageSource>
ts::messages::Message_center<InMessageType, OutMessageType>::Message_center(MessageSource message_source)
: message_source_(std::move(message_source))
{
}

template <typename InMessageType, typename OutMessageType>
void ts::messages::Message_center<InMessageType, OutMessageType>::handle_messages() const
{
    while (message_source_(message_buffer_))
    {
        handle_message(message_buffer_);
    }
}


#endif