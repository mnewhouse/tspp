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

#include "graphics/colored_text.hpp"

namespace ts
{
    namespace cup    
    {
        using Chat_message = graphics::Colored_text;
        using Composite_message = graphics::Composite_text;
        using Message_color = sf::Color;

        class Chatbox;

        struct Chatbox_listener
        {
            Chatbox_listener(Chatbox* chatbox);
            Chatbox_listener(const Chatbox_listener&);

            Chatbox_listener& operator=(const Chatbox_listener&) = default;

            virtual void on_chat_message(const Composite_message& message) {};

        protected:
            ~Chatbox_listener();

        private:
            Chatbox* chatbox_;
        };

        class Chatbox
        {
        public:
            void output_message(utf8_string message, Message_color color = sf::Color::Transparent);
            void output_message(Chat_message message);
            void output_message(const Composite_message& message);

            void add_chatbox_listener(Chatbox_listener* listener);
            void remove_chatbox_listener(Chatbox_listener* listener);

        private:
            std::vector<Chatbox_listener*> chatbox_listeners_;
        };
    }
}
#endif