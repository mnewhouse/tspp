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
#include "hud_chatbox.hpp"
#include "chatbox_display.hpp"

#include "client/client_interface.hpp"
#include "game/event_dispatcher.hpp"

#include "cup/chatbox.hpp"
#include "resources/settings/input_settings.hpp"

ts::hud::Chatbox::Chatbox(const resources::Input_settings& input_settings, const client::Client_interface* client_interface, gui::Context* context)
    : event_dispatcher_(nullptr),
      client_interface_(client_interface),
      chatbox_key_(input_settings.chatbox_key),
      chatbox_display_(std::make_unique<Chatbox_display>(client_interface->chatbox(), context))
{
    hide();
}

ts::hud::Chatbox::~Chatbox()
{
}

void ts::hud::Chatbox::show()
{
    chatbox_display_->show_chatbox();
}

void ts::hud::Chatbox::hide()
{
    chatbox_display_->hide_chatbox();
}

void ts::hud::Chatbox::update(std::size_t frame_duration)
{
    chatbox_display_->update();
}

void ts::hud::Chatbox::start_listening(game::Event_dispatcher* event_dispatcher)
{
    event_dispatcher_ = event_dispatcher;
    game::Scoped_event_handler::start_listening(event_dispatcher);
}

void ts::hud::Chatbox::send_chat_message(const utf8_string& message)
{
    client_interface_->write_message(message);
}

void ts::hud::Chatbox::handle_event(const sf::Event& event)
{
    if (!event_hog_)
    {
        if (event.type == sf::Event::KeyReleased)
        {
            if (event.key.code == chatbox_key_)
            {
                event_hog_ = event_dispatcher_->acquire_event_hog(this);

                chatbox_display_->toggle_chatbox_input(true);
            }
        }
    }

    else
    {
        if (event.type == sf::Event::KeyReleased)
        {
            if (event.key.code == sf::Keyboard::Return)
            {
                send_chat_message(chatbox_display_->chatbox_text());

                chatbox_display_->toggle_chatbox_input(false);
                event_hog_ = Generic_scope_exit();
            }

            else if (event.key.code == sf::Keyboard::Escape)
            {
                chatbox_display_->toggle_chatbox_input(false);
                event_hog_ = Generic_scope_exit();
            }
        }
    }
}