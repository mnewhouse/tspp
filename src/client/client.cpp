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
#include "client.hpp"
#include "client_messages.hpp"
#include "client_interface.hpp"
#include "client_interactions.hpp"
#include "client_stage_interface.hpp"
#include "client_control_interface.hpp"

#include "cup/cup.hpp"
#include "network/client_connection.hpp"

namespace ts
{
    namespace client
    {
        class Client_message_dispatcher
            : public Message_dispatcher
        {
        public:
            Client_message_dispatcher(network::Client_connection* client_connection, Message_center* message_center);

            virtual void dispatch_message(const Server_message& message) override;
            
        private:
            network::Client_connection* client_connection_;
        };
    }
}

ts::client::Client_message_dispatcher::Client_message_dispatcher(network::Client_connection* client_connection, Message_center* message_center)
: Message_dispatcher(message_center),
  client_connection_(client_connection)
{
}

void ts::client::Client_message_dispatcher::dispatch_message(const Server_message& message)
{
    if (message.message_type == Message_type::Fast)
    {
        client_connection_->send_udp_message(message.message);
    }

    else
    {
        client_connection_->send_tcp_message(message.message);
    }
}

class ts::client::impl::Client
{
public:
    Client(resources::Resource_store* resource_store);

    void poll();

    network::Client_connection client_connection_;
    cup::Cup cup_;
    Message_center message_center_;
    Client_message_dispatcher message_dispatcher_;

    Client_interface client_interface_;
    Interaction_interface interaction_interface_;
    Stage_interface stage_interface_;

    Server_message message_buffer_;
    std::unique_ptr<Control_interface> control_interface_;
};

ts::client::impl::Client::Client(resources::Resource_store* resource_store)
: client_connection_(),
  cup_(cup::Locality::Remote),
  message_center_(),
  message_dispatcher_(&client_connection_, &message_center_),
  client_interface_(&message_center_, &cup_),
  interaction_interface_(&message_center_, &cup_, resource_store),
  stage_interface_(&message_center_)
{
}

void ts::client::impl::Client::poll()
{
    while (client_connection_.get_tcp_message(message_buffer_.message))
    {
        message_buffer_.message_type = Message_type::Reliable;
        message_center_.handle_message(message_buffer_);
    }

    while (client_connection_.get_udp_message(message_buffer_.message))
    {
        message_buffer_.message_type = Message_type::Fast;
        message_center_.handle_message(message_buffer_);
    }
}

ts::client::Client::Client(resources::Resource_store* resource_store)
: impl_(std::make_unique<impl::Client>(resource_store))
{
}

ts::client::Client::~Client()
{
}

void ts::client::Client::clean_stage()
{
    impl_->stage_interface_.clean_stage();
}

void ts::client::Client::launch_action()
{
    impl_->stage_interface_.launch_action();
}

void ts::client::Client::end_action()
{
    if (impl_->cup_.cup_state() == cup::Cup_state::Action)
    {
        impl_->cup_.advance();
    }
}

void ts::client::Client::update(std::size_t frame_duration)
{
    impl_->poll();

    impl_->stage_interface_.update(frame_duration);
}

ts::controls::Control_interface* ts::client::Client::make_control_interface()
{
    const auto* stage = impl_->stage_interface_.stage();
    const auto* message_center = &impl_->message_center_;

    impl_->control_interface_ = std::make_unique<Control_interface>(stage, message_center);
    return impl_->control_interface_.get();
}

const ts::client::Client_interface* ts::client::Client::client_interface() const
{
    return &impl_->client_interface_;
}

ts::client::Connection_status ts::client::Client::connection_status() const
{
    return impl_->client_connection_.connection_status();
}

ts::client::Registration_status ts::client::Client::registration_status() const
{
    return impl_->interaction_interface_.registration_status();
}

const ts::utf8_string& ts::client::Client::registration_error() const
{
    return impl_->interaction_interface_.registration_error();
}

void ts::client::Client::add_cup_listener(cup::Cup_listener* listener)
{
    impl_->cup_.add_cup_listener(listener);
}

void ts::client::Client::remove_cup_listener(cup::Cup_listener* listener)
{
    impl_->cup_.remove_cup_listener(listener);
}

const ts::cup::Chatbox* ts::client::Client::chatbox() const
{
    return impl_->interaction_interface_.chatbox();
}

void ts::client::Client::add_chatbox_listener(cup::Chatbox_listener* listener)
{
    impl_->interaction_interface_.add_chatbox_listener(listener);
}

void ts::client::Client::remove_chatbox_listener(cup::Chatbox_listener* listener)
{
    impl_->interaction_interface_.remove_chatbox_listener(listener);
}


void ts::client::Client::async_connect(utf8_string remote_address, std::uint16_t remote_port)
{
    impl_->client_connection_.async_connect(std::move(remote_address), remote_port);
}

void ts::client::Client::send_registration_request()
{
    impl_->interaction_interface_.send_registration_request();
}

const ts::game::Stage_loader* ts::client::Client::async_load_stage(const cup::Stage_data& stage_data, 
                                                                   std::function<void(const action::Stage*)> completion_callback)
{
    return impl_->stage_interface_.async_load_stage(stage_data, completion_callback);
}