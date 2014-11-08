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
#include "chatbox_display.hpp"

#include "client/client_interface.hpp"

#include "resources/font_library.hpp"

#include "graphics/text.hpp"

#include "user_interface/gui_context.hpp"
#include "user_interface/document.hpp"
#include "user_interface/document_handle.hpp"
#include "user_interface/elements/elements.hpp"

#include "cup/chatbox.hpp"
#include "cup/cup_messages.hpp"

struct ts::game::Chatbox_display::Displayed_message
{
    Displayed_message(const cup::Composite_message& message, 
        std::chrono::high_resolution_clock::time_point time_point = std::chrono::high_resolution_clock::now())
    : message(message),
      time_point(time_point)
    {}

    cup::Composite_message message;
    std::chrono::high_resolution_clock::time_point time_point;
};

struct ts::game::Chatbox_display::Impl
    : public client::Message_listener
{
    Impl(const cup::Chatbox& chatbox, const client::Client_interface* client_interface, gui::Context* context);

    void populate_chatbox(const cup::Chatbox& chatbox);
    void create_document(gui::Context* context);
    void display_message(const graphics::Composite_text& message);

    virtual void handle_message(const client::Server_message& server_message) override;
    void update();
    
    const client::Client_interface* client_interface_;

    std::deque<Displayed_message> displayed_messages_;

    std::vector<gui::Text_element*> chat_lines_;
    gui::Edit_box* chat_input_;

    gui::Document_handle document_;
};

ts::game::Chatbox_display::Impl::Impl(const cup::Chatbox& chatbox, const client::Client_interface* client_interface, gui::Context* context)
: Message_listener(client_interface->message_center()),
  client_interface_(client_interface)
{
    populate_chatbox(chatbox);
    create_document(context);
}

void ts::game::Chatbox_display::Impl::populate_chatbox(const cup::Chatbox& chatbox)
{
    const auto back_log = chatbox.backlog();
    auto now = std::chrono::high_resolution_clock::now();
    auto display_duration = std::chrono::seconds(6);
    std::size_t counter = 0;

    for (auto it = back_log.begin(); counter != 3 && it != back_log.end() && now - it->time_stamp < display_duration; ++it, ++counter)
    {
        displayed_messages_.emplace_front(it->message, it->time_stamp);
    }
}

void ts::game::Chatbox_display::Impl::create_document(gui::Context* context)
{
    document_ = context->create_document("ingame-chatbox");
    document_->set_visible(false);

    auto screen_size = context->screen_size();

    const auto& font_library = context->font_library();

    gui::Text_style text_style;
    text_style.character_size = 14;
    text_style.font = font_library.font_by_name(resources::fonts::Monospace);
    text_style.color = sf::Color::White;
    
    for (std::size_t index = 0; index != 3; ++index)
    {
        auto chat_line = document_->create_child<gui::Text_element>("", text_style);
        chat_line->set_position(Vector2<double>(5.0, index * 20.0));
        if (index < displayed_messages_.size())
        {
            chat_line->set_text(displayed_messages_[index].message);
        }

        chat_lines_.push_back(chat_line);
    }

    chat_input_ = document_->create_child<gui::Edit_box>(text_style);
    chat_input_->set_position({ 5.0, screen_size.y - 20.0 });
    chat_input_->set_active(false);
}

ts::game::Chatbox_display::Chatbox_display(const cup::Chatbox& chatbox, const client::Client_interface* client_interface, gui::Context* context)
: impl_(std::make_unique<Impl>(chatbox, client_interface, context))
{
}

ts::game::Chatbox_display::~Chatbox_display()
{
}

void ts::game::Chatbox_display::Impl::handle_message(const client::Server_message& server_message)
{
    const auto& message = server_message.message;
    if (server_message.message_type == client::Message_type::Reliable &&
        message.type() == cup::Message_type::chatbox_output)
    {
        auto message_info = cup::parse_chatbox_output_message(message);
        display_message(message_info.message);
    }
}

void ts::game::Chatbox_display::Impl::display_message(const graphics::Composite_text& message)
{
    auto& messages = displayed_messages_;
    messages.emplace_back(message);

    while (messages.size() > 3)
    {
        messages.pop_front();
    }

    update();
}

void ts::game::Chatbox_display::Impl::update()
{
    auto now = std::chrono::high_resolution_clock::now();
    
    auto& messages = displayed_messages_;    
    while (!messages.empty() && now - messages.front().time_point >= std::chrono::seconds(6))
    {
        messages.pop_front();
    }

    for (std::size_t index = 0; index != chat_lines_.size(); ++index)
    {
        auto chat_line = chat_lines_[index];
        chat_line->set_text(index < messages.size() ? messages[index].message : graphics::Composite_text());
    }
}

void ts::game::Chatbox_display::update()
{
    impl_->update();
}

void ts::game::Chatbox_display::show_chatbox()
{
    impl_->document_->set_visible(true);
}

void ts::game::Chatbox_display::hide_chatbox()
{
    impl_->document_->set_visible(false);
}

bool ts::game::Chatbox_display::chatbox_input_active() const
{
    return impl_->chat_input_->active();
}

void ts::game::Chatbox_display::send_chat_message()
{
    const auto& text = impl_->chat_input_->text();
    if (!text.empty())
    {
        impl_->client_interface_->write_message(impl_->chat_input_->text());
    }

    impl_->chat_input_->set_active(false);
    impl_->chat_input_->reset();
}

void ts::game::Chatbox_display::toggle_chatbox_input(bool enable)
{
    impl_->chat_input_->set_active(enable);
}