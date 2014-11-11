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

#ifndef CUP_CHATBOX_HPP
#define CUP_CHATBOX_HPP

#include "chat_message.hpp"

namespace ts
{
    namespace cup
    {
        struct Chatbox_listener;

        struct Chatbox_log_entry
        {
            Chat_message message;
            std::chrono::high_resolution_clock::time_point time_stamp;
        };

        class Chatbox
        {
        public:
            void dispatch_message(const Chat_message& message);

            void add_chatbox_listener(Chatbox_listener* listener);
            void remove_chatbox_listener(Chatbox_listener* listener);

            void set_max_backlog_size(std::size_t size);
            std::size_t max_backlog_size() const;

            using backlog_iterator = std::deque<Chatbox_log_entry>::const_reverse_iterator;
            Range<backlog_iterator> backlog() const;


        private:
            void log_message(const Chat_message& message);

            std::vector<Chatbox_listener*> listeners_;

            std::size_t max_backlog_size_ = 256;
            std::deque<Chatbox_log_entry> message_log_;
        };
    }
}

#endif
