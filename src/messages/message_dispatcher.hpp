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
        class Dispatcher_set;

        template <typename MessageType>
        class Message_dispatcher
        {
        public:
            using dispatcher_set_type = Dispatcher_set<MessageType>;
            Message_dispatcher(dispatcher_set_type* = nullptr);

            virtual ~Message_dispatcher();

            Message_dispatcher(const Message_dispatcher<MessageType>& other);
            Message_dispatcher<MessageType>& operator=(const Message_dispatcher<MessageType>& other);

            virtual void dispatch_message(const MessageType& message) = 0;            

        private:
            dispatcher_set_type* dispatcher_set_;
        };
        
        template <typename MessageType>
        class Dispatcher_set
        {
        public:
            using dispatcher_type = Message_dispatcher<MessageType>;

            void dispatch_message(const MessageType& message) const;

            void add_message_dispatcher(dispatcher_type* dispatcher);
            void remove_message_dispatcher(dispatcher_type* dispatcher);

        private:
            std::vector<dispatcher_type*> message_dispatchers_;
        };
    }
}


template <typename MessageType>
ts::messages::Message_dispatcher<MessageType>::Message_dispatcher(dispatcher_set_type* dispatcher_set)
: dispatcher_set_(dispatcher_set)
{
    if (dispatcher_set)
    {
        dispatcher_set->add_message_dispatcher(this);
    }
}

template <typename MessageType>
ts::messages::Message_dispatcher<MessageType>::~Message_dispatcher()
{
    if (dispatcher_set_)
    {
        dispatcher_set_->remove_message_dispatcher(this);
    }
}

template <typename MessageType>
ts::messages::Message_dispatcher<MessageType>::Message_dispatcher(const Message_dispatcher<MessageType>& other)
: dispatcher_set_(other.dispatcher_set_)
{
    if (dispatcher_set_)
    {
        dispatcher_set_->add_message_dispatcher(this);
    }
}

template <typename MessageType>
ts::messages::Message_dispatcher<MessageType>& ts::messages::Message_dispatcher<MessageType>::operator=(const Message_dispatcher<MessageType>& other)
{
    dispatcher_set_ = other.dispatcher_set_;

    if (dispatcher_set_)
    {
        dispatcher_set_->add_message_dispatcher(this);
    }

    return *this;
}

template <typename MessageType>
void ts::messages::Dispatcher_set<MessageType>::dispatch_message(const MessageType& message) const
{
    for (auto dispatcher : message_dispatchers_)
    {
        dispatcher->dispatch_message(message);
    }
}

template <typename MessageType>
void ts::messages::Dispatcher_set<MessageType>::add_message_dispatcher(dispatcher_type* dispatcher)
{
    message_dispatchers_.push_back(dispatcher);
}

template <typename MessageType>
void ts::messages::Dispatcher_set<MessageType>::remove_message_dispatcher(dispatcher_type* dispatcher)
{
    message_dispatchers_.erase(std::remove(message_dispatchers_.begin(), message_dispatchers_.end(), dispatcher), message_dispatchers_.end());
}

#endif