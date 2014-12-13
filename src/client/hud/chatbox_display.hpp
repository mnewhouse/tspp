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

#ifndef CHATBOX_DISPLAY_HPP
#define CHATBOX_DISPLAY_HPP

#include "cup/chatbox_listener.hpp"

namespace ts
{
    namespace cup
    {
        class Chatbox;
    }

    namespace gui
    {
        class Context;
    }

    namespace graphics
    {
        class Composite_text;
    }

    namespace hud
    {
        class Chatbox_display
        {
        public:
            Chatbox_display(const cup::Chatbox* chatbox, gui::Context* context);
            ~Chatbox_display();
            
            void display_message(const graphics::Composite_text& message);
            void update();

            void toggle_chatbox_input(bool enable);
            bool chatbox_input_active() const;

            const utf8_string& chatbox_text() const;

            void show_chatbox();
            void hide_chatbox();

        private:
            struct Impl;
            struct Displayed_message;

            std::unique_ptr<Impl> impl_;
        };
    }
}

#endif