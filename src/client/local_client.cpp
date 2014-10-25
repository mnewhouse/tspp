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
#include "local_client.hpp"
#include "local_players.hpp"

#include "client_interface.hpp"
#include "client_messages.hpp"
#include "client_control_interface.hpp"
#include "chatbox_interface.hpp"

#include "server/server.hpp"

#include "cup/chatbox.hpp"

#include "resources/resource_store.hpp"

namespace ts
{
    namespace client
    {
        class Local_message_dispatcher
            : public Message_dispatcher
        {
        public:
            Local_message_dispatcher(Message_center* message_center, const server::Message_center* server_message_center);

            virtual void dispatch_message(const Server_message& message) override;

        private:
            const server::Message_center* server_message_center_;
            server::Client_message message_buffer_;
        };
    }
}

class ts::client::Local_client::Impl
{
public:
    Impl(const server::Server* server, resources::Resource_store* resource_store);

    Message_center message_center_;
    Client_interface client_interface_;
    Local_message_dispatcher message_dispatcher_;

    Chatbox_interface chatbox_interface_;

    std::unique_ptr<Control_interface> control_interface_;    
};

ts::client::Local_client::Impl::Impl(const server::Server* server, resources::Resource_store* resource_store)
: message_center_(),
  client_interface_(&message_center_, server->cup()),
  message_dispatcher_(&message_center_, server->message_center()),
  chatbox_interface_(&message_center_)
{
}


ts::client::Local_message_dispatcher::Local_message_dispatcher(Message_center* message_center, const server::Message_center* server_message_center)
: Message_dispatcher(message_center),
  server_message_center_(server_message_center)
{
}

void ts::client::Local_message_dispatcher::dispatch_message(const Server_message& message)
{
    message_buffer_.client = server::Generic_client(server::local_client);
    message_buffer_.message = message.message;
    message_buffer_.message_type = (message.message_type == Message_type::Fast ? server::Message_type::Fast : server::Message_type::Reliable);

    server_message_center_->handle_message(message_buffer_);
}

ts::client::Local_client_link::Local_client_link(const Local_client* local_client)
: local_client_(local_client)
{
}

void ts::client::Local_client_link::operator()(const server::Client_message& message) const
{
    message_buffer_.message = message.message;
    message_buffer_.message_type = (message.message_type == server::Message_type::Fast ? Message_type::Fast : Message_type::Reliable);

    local_client_->impl_->message_center_.handle_message(message_buffer_);
}

ts::client::Local_client::Local_client(server::Server* server, resources::Resource_store* resource_store)
: impl_(std::make_unique<Impl>(server, resource_store))
{
    auto local_players = create_local_player_definitions(resource_store->player_settings(), resource_store->player_store());

    server->register_local_client(Local_client_link(this), local_players);
}

ts::client::Local_client::~Local_client()
{
}

const ts::client::Client_interface* ts::client::Local_client::client_interface() const
{
    return &impl_->client_interface_;
}

ts::controls::Control_interface* ts::client::Local_client::make_control_interface(const action::Stage* stage)
{
    impl_->control_interface_ = std::make_unique<Control_interface>(stage, &impl_->message_center_);
    return impl_->control_interface_.get();
}

const ts::cup::Chatbox* ts::client::Local_client::chatbox() const
{
    return impl_->chatbox_interface_.chatbox();
}

void ts::client::Local_client::add_chatbox_listener(cup::Chatbox_listener* listener)
{
    impl_->chatbox_interface_.add_chatbox_listener(listener);
}

void ts::client::Local_client::remove_chatbox_listener(cup::Chatbox_listener* listener)
{
    impl_->chatbox_interface_.remove_chatbox_listener(listener);
}