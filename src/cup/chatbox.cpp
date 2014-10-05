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

ts::cup::Chatbox_listener::Chatbox_listener(Chatbox* chatbox)
: chatbox_(chatbox)
{
    chatbox->add_chatbox_listener(this);
}

ts::cup::Chatbox_listener::~Chatbox_listener()
{
    chatbox_->remove_chatbox_listener(this);
}

ts::cup::Chatbox_listener::Chatbox_listener(const Chatbox_listener& other)
: chatbox_(other.chatbox_)
{
    chatbox_->add_chatbox_listener(this);
}

void ts::cup::Chatbox::output_message(utf8_string message, Message_color color)
{
    output_message(Chat_message(std::move(message), color));
}

void ts::cup::Chatbox::output_message(Chat_message message)
{
    output_message(Composite_message(std::move(message)));
}

void ts::cup::Chatbox::output_message(const Composite_message& message)
{
    for (auto listener : chatbox_listeners_)
    {
        listener->on_chat_message(message);
    }
}

void ts::cup::Chatbox::add_chatbox_listener(Chatbox_listener* listener)
{
    chatbox_listeners_.push_back(listener);
}

void ts::cup::Chatbox::remove_chatbox_listener(Chatbox_listener* listener)
{
    chatbox_listeners_.erase(std::remove(chatbox_listeners_.begin(), chatbox_listeners_.end(), listener), chatbox_listeners_.end());
}


ts::cup::Composite_message ts::cup::format_chat_message(const utf8_string& sender, const utf8_string& message)
{
    return Chat_message("<" + sender + "> ", sf::Color(150, 200, 255)) + Chat_message(message, sf::Color(255, 255, 255));
}