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
        template <typename MessageType> class Listener_set;

        template <typename MessageType>
        class Message_listener
        {
        public:
            using listener_set_type = Listener_set<MessageType>;

            Message_listener(listener_set_type* listener_set = nullptr);
            virtual ~Message_listener();
            virtual void handle_message(const MessageType& message) = 0;

            Message_listener(const Message_listener<MessageType>& other);
            Message_listener<MessageType>& operator=(const Message_listener<MessageType>& rhs);

        private:
            listener_set_type* listener_set_;
        };

        template <typename MessageType>
        class Listener_set
        {
        public:
            void handle_message(const MessageType& message) const;

            using listener_type = Message_listener<MessageType>;
            void add_message_listener(listener_type* listener);
            void remove_message_listener(listener_type* listener);

        private:
            std::vector<listener_type*> message_listeners_;
        };
    }
}

template <typename MessageType>
ts::messages::Message_listener<MessageType>::Message_listener(listener_set_type* listener_set)
: listener_set_(listener_set)
{
    if (listener_set)
    {
        listener_set->add_message_listener(this);
    }
}

template <typename MessageType>
ts::messages::Message_listener<MessageType>::~Message_listener()
{
    if (listener_set_)
    {
        listener_set_->remove_message_listener(this);
    }
}

template <typename MessageType>
ts::messages::Message_listener<MessageType>::Message_listener(const Message_listener<MessageType>& other)
: listener_set_(other.listener_set_)
{
    if (listener_set_)
    {
        listener_set_->add_message_listener(this);
    }
}

template <typename MessageType>
ts::messages::Message_listener<MessageType>& ts::messages::Message_listener<MessageType>::operator=(const Message_listener<MessageType>& other)
{
    listener_set_ = other.listener_set_;

    if (listener_set_)
    {
        listener_set_->add_message_listener(this);
    }

    return *this;
}

template <typename MessageType>
void ts::messages::Listener_set<MessageType>::handle_message(const MessageType& message) const
{
    for (auto listener : message_listeners_)
    {
        listener->handle_message(message);
    }
}

template <typename MessageType>
void ts::messages::Listener_set<MessageType>::add_message_listener(listener_type* listener)
{
    message_listeners_.push_back(listener);
}

template <typename MessageType>
void ts::messages::Listener_set<MessageType>::remove_message_listener(listener_type* listener)
{
    message_listeners_.erase(std::remove(message_listeners_.begin(), message_listeners_.end(), listener), message_listeners_.end());
}
    

#endif