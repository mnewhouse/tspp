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
#include "chatbox.hpp"
#include "chatbox_listener.hpp"

void ts::cup::Chatbox::add_chatbox_listener(Chatbox_listener* listener)
{
    listeners_.push_back(listener);
}

void ts::cup::Chatbox::remove_chatbox_listener(Chatbox_listener* listener)
{
    listeners_.erase(std::remove(listeners_.begin(), listeners_.end(), listener), listeners_.end());
}

void ts::cup::Chatbox::dispatch_message(const Chat_message& message)
{
    for (auto listener : listeners_)
    {
        listener->on_chat_message(message);
    }

    log_message(message);
}

void ts::cup::Chatbox::log_message(const Chat_message& message)
{
    message_log_.emplace_back();
    message_log_.back().message = message;
    message_log_.back().time_stamp = std::chrono::high_resolution_clock::now();

    while (message_log_.size() > max_backlog_size_)
    {
        message_log_.pop_front();
    }
}

ts::Range<ts::cup::Chatbox::backlog_iterator> ts::cup::Chatbox::backlog() const
{
    return Range<backlog_iterator>(message_log_.rbegin(), message_log_.rend());
}