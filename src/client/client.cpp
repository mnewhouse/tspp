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
#include "common_client_logic.hpp"

#include "client_stage_interface.hpp"
#include "client_control_interface.hpp"
#include "client_interactions.hpp"

#include "cup/cup.hpp"
#include "network/client_connection.hpp"

#include "resources/resource_store.hpp"

namespace ts
{
    namespace client
    {
        class Client_message_dispatcher
            : public Scoped_message_dispatcher
        {
        public:
            Client_message_dispatcher(network::Client_connection* client_connection, Message_center* message_center);

            virtual void dispatch_message(const Server_message& message) override;
            
        private:
            network::Client_connection* client_connection_;
        };

        struct Cup_container
        {
            Cup_container()
                : cup_(cup::Locality::Remote)
            {
            }

            cup::Cup cup_;
        };
    }
}

ts::client::Client_message_dispatcher::Client_message_dispatcher(network::Client_connection* client_connection, Message_center* message_center)
: Scoped_message_dispatcher(message_center),
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
    : public Cup_container, public Common_logic
{
public:
    Client(resources::Resource_store* resource_store);

    void poll();

    network::Client_connection client_connection_;   
    
    Stage_interface stage_interface_;
    Interaction_interface interaction_interface_;

    Client_message_dispatcher message_dispatcher_;
    Server_message message_buffer_;
};

ts::client::impl::Client::Client(resources::Resource_store* resource_store)
: Cup_container(),
  Common_logic(&cup_, resource_store),
  client_connection_(),
  stage_interface_(&message_center_, &resource_store->network_settings()),
  interaction_interface_(&message_center_, &cup_, resource_store),
  message_dispatcher_(&client_connection_, &message_center_)
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

    interaction_interface_.poll();

    try
    {
        stage_interface_.poll();
    }

    catch (const std::exception& error)
    {
        client_interface_.load_error(error.what());
    }
}

ts::client::Client::Client(resources::Resource_store* resource_store)
: impl_(std::make_unique<impl::Client>(resource_store))
{
}

ts::client::Client::~Client()
{
}

ts::Generic_scope_exit ts::client::Client::launch_action()
{
    impl_->stage_interface_.launch_action();

    return Generic_scope_exit([this]() { end_action(); });
}

void ts::client::Client::end_action()
{
    impl_->stage_interface_.clear();
}

void ts::client::Client::update(std::size_t frame_duration)
{
    impl_->poll();
    impl_->update(frame_duration);

    impl_->stage_interface_.update(frame_duration);
}

void ts::client::Client::on_render()
{
    impl_->on_render();
}

std::unique_ptr<ts::controls::Control_interface> ts::client::Client::make_control_interface() const
{
    const auto* stage = impl_->stage_interface_.stage();
    const auto* message_center = &impl_->message_center_;

    return std::make_unique<Control_interface>(stage, message_center);
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

void ts::client::Client::async_connect(utf8_string remote_address, std::uint16_t remote_port)
{
    impl_->client_connection_.async_connect(std::move(remote_address), remote_port);
}

void ts::client::Client::send_registration_request()
{
    impl_->interaction_interface_.send_registration_request();
}

bool ts::client::Client::is_downloading() const
{
    return impl_->interaction_interface_.is_downloading();
}

std::pair<std::size_t, std::size_t> ts::client::Client::download_progress() const
{
    return impl_->interaction_interface_.download_progress();
}

const ts::resources::Loading_interface* ts::client::Client::async_load_stage(const cup::Stage_data& stage_data, 
                                                                             std::function<void()> completion_callback)
{
    // Hook the callback, so that we can inform the server that we are ready
    auto completion_hook = [this, completion_callback]()
    {
        impl_->client_interface_.signal_ready();

        completion_callback();       
    };

    auto loading_sequence = &impl_->loading_sequence_;
    loading_sequence->set_completion_handler(completion_hook);

    // Make sure the loading sequence loads the stage too, by calling the appropriate overload.
    loading_sequence->async_load(stage_data, impl_->stage_interface_.base());

    return loading_sequence;
}

const ts::action::Stage* ts::client::Client::stage() const
{
    return impl_->stage_interface_.stage();
}

std::shared_ptr<ts::scene::Scene> ts::client::Client::acquire_scene()
{
    auto shared_scene = std::make_shared<scene::Scene>(impl_->loading_sequence_.transfer_result());
    impl_->scene_interface_.reset(shared_scene);
    return shared_scene;
}