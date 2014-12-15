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
#include "common_client_logic.hpp"

#include "client_control_interface.hpp"

#include "server/server.hpp"

#include "resources/resource_store.hpp"

namespace ts
{
    namespace client
    {
        class Local_message_dispatcher
            : public Scoped_message_dispatcher
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
    : public Common_logic
{
public:
    Impl(const server::Server* server, resources::Resource_store* resource_store);

    Local_message_dispatcher message_dispatcher_;
};

ts::client::Local_client::Impl::Impl(const server::Server* server, resources::Resource_store* resource_store)
    : Common_logic(server->cup(), resource_store),
      message_dispatcher_(&message_center_, server->message_center())
{
}


ts::client::Local_message_dispatcher::Local_message_dispatcher(Message_center* message_center, const server::Message_center* server_message_center)
: Scoped_message_dispatcher(message_center),
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

std::unique_ptr<ts::controls::Control_interface> ts::client::Local_client::make_control_interface(const action::Stage* stage) const
{
    return std::make_unique<Control_interface>(stage, &impl_->message_center_);
}

void ts::client::Local_client::update(std::size_t frame_duration)
{
    impl_->update(frame_duration);
}

void ts::client::Local_client::on_render()
{
    impl_->on_render();
}


const ts::resources::Loading_interface* ts::client::Local_client::async_load_scene(const action::Stage* stage,
                                                                                   std::function<void()> callback)
{
    auto completion_handler = [this, callback]()
    {
        impl_->client_interface_.signal_ready();

        callback();
    };

    impl_->loading_sequence_.set_completion_handler(completion_handler);
    impl_->loading_sequence_.async_load(stage);

    return &impl_->loading_sequence_;
}


std::shared_ptr<ts::scene::Scene> ts::client::Local_client::acquire_scene()
{
    return impl_->acquire_scene();
}