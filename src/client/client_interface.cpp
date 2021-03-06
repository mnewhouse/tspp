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
#include "client_interface.hpp"

#include "cup/cup_messages.hpp"

ts::client::Client_interface::Client_interface(Message_center* message_center, const cup::Cup* cup, 
    const cup::Chatbox* chatbox, const resources::Resource_store* resource_store)
: message_center_(message_center),
  cup_(cup),
  chatbox_(chatbox),
  resource_store_(resource_store)
{
}

ts::client::Client_interface::~Client_interface()
{
    // Send quit message when this thing is destroyed.
    quit();
}

void ts::client::Client_interface::select_cars(const std::vector<Car_selection>& car_selection) const
{
    message_buffer_.message = cup::make_car_selection_message(car_selection);    
    message_center_->dispatch_message(message_buffer_);
}

void ts::client::Client_interface::write_message(const utf8_string& message) const
{
    message_buffer_.message = cup::make_chat_message(message);
    message_center_->dispatch_message(message_buffer_);
}

void ts::client::Client_interface::signal_ready() const
{
    message_buffer_.message = cup::make_ready_signal_message();
    message_center_->dispatch_message(message_buffer_);
}

void ts::client::Client_interface::request_advance() const
{
    message_buffer_.message = cup::make_advance_request_message();
    message_center_->dispatch_message(message_buffer_);
}

void ts::client::Client_interface::quit() const
{
    message_buffer_.message = cup::make_client_quit_message();
    message_center_->dispatch_message(message_buffer_);
}

void ts::client::Client_interface::load_error(const utf8_string& error_message) const
{
    message_buffer_.message = cup::make_load_error_message(error_message);
    message_center_->handle_message(message_buffer_);
}

ts::client::Message_center* ts::client::Client_interface::message_center() const
{
    return message_center_;
}

const ts::cup::Cup* ts::client::Client_interface::cup() const
{
    return cup_;
}

const ts::cup::Chatbox* ts::client::Client_interface::chatbox() const
{
    return chatbox_;
}

const ts::resources::Resource_store* ts::client::Client_interface::resource_store() const
{
    return resource_store_;
}