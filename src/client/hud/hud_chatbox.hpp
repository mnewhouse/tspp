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

#ifndef CHATBOX_EVENT_HANDLER_HPP
#define CHATBOX_EVENT_HANDLER_HPP

#include "game/event_handler.hpp"

namespace ts
{
    namespace game
    {
        class Event_dispatcher;
    }

    namespace resources
    {
        class Input_settings;
    }

    namespace gui
    {
        class Context;
    }

    namespace client
    {
        class Client_interface;
    }

    namespace hud
    {
        class Chatbox_display;

        class Chatbox
            : private game::Scoped_event_handler
        {
        public:
            Chatbox(const resources::Input_settings& input_settings, const client::Client_interface* client_interface,gui::Context* context);

            ~Chatbox();

            virtual void handle_event(const sf::Event& event) override;

            void show();
            void hide();

            void update(std::size_t frame_duration);

            void start_listening(game::Event_dispatcher* event_dispatcher);
            using game::Scoped_event_handler::stop_listening;

        private:
            void send_chat_message(const utf8_string& message);

            std::unique_ptr<Chatbox_display> chatbox_display_;
            const client::Client_interface* client_interface_;

            game::Event_dispatcher* event_dispatcher_;
            Generic_scope_exit event_hog_;

            sf::Keyboard::Key chatbox_key_ = sf::Keyboard::T;
            
        };
    }
}

#endif