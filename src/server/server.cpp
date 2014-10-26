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
#include "server.hpp"
#include "client_map.hpp"
#include "server_interactions.hpp"
#include "server_stage_interface.hpp"

#include "cup/cup.hpp"
#include "cup/cup_config.hpp"

#include "network/server_connection.hpp"

namespace ts
{
    namespace server
    {
        class Server_message_dispatcher
            : public messages::Message_dispatcher<Client_message>
        {
        public:
            Server_message_dispatcher(impl::Server* server, Message_center* message_center);

            virtual void dispatch_message(const Client_message& message) override;

        private:
            impl::Server* server_;
        };
    }
}

class ts::server::impl::Server
{
public:
    Server(resources::Resource_store* resource_store);

    void poll();

    void dispatch_message(const Client_message& message);
    void end_action();

    cup::Cup cup_;
    cup::Cup_config cup_config_;
    network::Server_connection server_connection_;

    Message_center message_center_;
    Server_message_dispatcher message_dispatcher_;
    Client_message message_buffer_;

    Client_map client_map_;
    Interaction_interface interaction_interface_;
    Stage_interface stage_interface_;

    using Local_client_link = std::function<void(const Client_message&)>;
    Local_client_link local_client_link_;
};

ts::server::Server_message_dispatcher::Server_message_dispatcher(impl::Server* server, Message_center* message_center)
: Message_dispatcher(message_center),
  server_(server)
{
}

void ts::server::Server_message_dispatcher::dispatch_message(const Client_message& message)
{
    server_->dispatch_message(message);
}

ts::server::impl::Server::Server(resources::Resource_store* resource_store)
: cup_(cup::Locality::Local),
  cup_config_(&cup_, resource_store),
  server_connection_(),
  message_center_(),
  message_dispatcher_(this, &message_center_),
  client_map_(&cup_, &server_connection_),
  interaction_interface_(&message_center_, &client_map_, &cup_, resource_store),
  stage_interface_(&message_center_, &cup_)
{
}

void ts::server::impl::Server::dispatch_message(const Client_message& message)
{
    if (message.message_type == Message_type::Reliable)
    {
        switch (message.client.type())
        {
        case Generic_client::None:
            server_connection_.send_tcp_message_to_all(message.message);
            break;

        case Generic_client::Handle:
            server_connection_.send_tcp_message(message.message, message.client.remote_handle());
            break;
        }
    }

    else if (message.message_type == Message_type::Fast)
    {
        switch (message.client.type())
        {
        case Generic_client::None:
            server_connection_.send_udp_message_to_all(message.message);
            break;

        case Generic_client::Address:
            server_connection_.send_udp_message(message.message, message.client.remote_address());
            break;

        case Generic_client::Handle:
            server_connection_.send_udp_message(message.message, message.client.remote_handle());
            break;
        }
    }

    if (local_client_link_)
    {
        switch (message.client.type())
        {
        case Generic_client::Local:
        case Generic_client::None:
            local_client_link_(message);
            break;
        }
    }
}

void ts::server::impl::Server::poll()
{
    while (server_connection_.accept_incoming_connection())
    {
        // Incoming connection!
        // We don't need to do anything though.
    }

    network::Client_handle client;
    while (server_connection_.get_tcp_message(message_buffer_.message, client))
    {
        message_buffer_.client = Generic_client(client);
        message_buffer_.message_type = Message_type::Reliable;

        message_center_.handle_message(message_buffer_);
    }

    network::Remote_address sender;
    while (server_connection_.get_udp_message(message_buffer_.message, sender))
    {
        message_buffer_.client = Generic_client(sender);
        message_buffer_.message_type = Message_type::Fast;
        message_center_.handle_message(message_buffer_);
    }

    interaction_interface_.poll();
}

void ts::server::Server::launch_action()
{
    impl_->stage_interface_.launch_action();
}

void ts::server::impl::Server::end_action()
{
    if (cup_.cup_state() == cup::Cup_state::Action)
    {
        cup_.advance();
    }
}

ts::server::Server::Server(resources::Resource_store* resource_store)
: impl_(std::make_unique<impl::Server>(resource_store))
{
}

ts::server::Server::~Server()
{
}

void ts::server::Server::update(std::size_t frame_duration)
{
    impl_->poll();
    impl_->stage_interface_.update(frame_duration);
}

void ts::server::Server::register_local_client(Local_client_link client_link, const std::vector<cup::Player_definition>& local_players)
{
    impl_->local_client_link_ = client_link;

    Generic_client client(local_client);
    impl_->client_map_.register_client(client);
    
    for (auto& player_def : local_players)
    {
        impl_->client_map_.register_player(client, player_def);
    }
}

const ts::server::Message_center* ts::server::Server::message_center() const
{
    return &impl_->message_center_;
}

const ts::cup::Cup* ts::server::Server::cup() const
{
    return &impl_->cup_;
}

void ts::server::Server::listen(std::uint16_t port)
{
    impl_->server_connection_.listen(port);
}

void ts::server::Server::remove_cup_listener(cup::Cup_listener* listener)
{
    impl_->cup_.remove_cup_listener(listener);
}

void ts::server::Server::add_cup_listener(cup::Cup_listener* listener)
{
    impl_->cup_.add_cup_listener(listener);
}

const ts::game::Stage_loader* ts::server::Server::async_load_stage(const cup::Stage_data& stage_data, std::function<void(const action::Stage*)> completion_callback)
{
    return impl_->stage_interface_.async_load_stage(stage_data, completion_callback);
}

void ts::server::Server::end_action()
{
    impl_->end_action();
}

void ts::server::Server::clean_stage()
{
    impl_->stage_interface_.clean_stage();
}

const ts::action::Stage* ts::server::Server::stage() const
{
    return impl_->stage_interface_.stage();
}