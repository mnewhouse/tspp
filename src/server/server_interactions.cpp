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
#include "server_interactions.hpp"
#include "server_interaction_listener.hpp"
#include "client_map.hpp"
#include "resource_download_server.hpp"

#include "cup/cup.hpp"
#include "cup/cup_messages.hpp"
#include "cup/chatbox.hpp"
#include "cup/cup_listener.hpp"

#include "resources/resource_store.hpp"

class ts::server::Interaction_interface::Impl
    : public Message_listener, public cup::Cup_listener
{
public:
    Impl(Message_center* message_center, Client_map* client_map, cup::Cup* cup, const resources::Resource_store* resource_store);
    ~Impl();

    void add_interaction_listener(Interaction_listener* listener);
    void remove_interaction_listener(Interaction_listener* listener);

    void broadcast_chat_message(const cup::Composite_message& message);
    void poll();

private:
    virtual void handle_message(const Client_message& client_message) override;
    virtual void on_state_change(cup::Cup_state old_state, cup::Cup_state new_state) override;
    virtual void on_initialize(const cup::Stage_data& stage_data) override;

    void handle_registration_request(const Client_message& client_message);
    void handle_bad_registration_request(const Client_message& client_message);

    void handle_advance_request(const Client_message& client_message);

    void handle_quit_message(const Client_message& client_message);
    void handle_chat_message(const Client_message& client_message);

    void disconnect_client(const Generic_client& client);
    utf8_string client_name(const Generic_client& client) const;

    void wait_for_everyone();
    void advance_if_ready();
    void handle_ready_signal(const Client_message& client_message);
    void handle_car_selection(const Client_message& client_message);

    Client_map* client_map_;
    Message_center* message_center_;
    cup::Cup* cup_;
    const resources::Resource_store* resource_store_;

    std::vector<Interaction_listener*> listeners_;
    std::unordered_set<Generic_client> awaiting_clients_;

    Resource_download_server download_server_;
};

ts::server::Interaction_interface::Interaction_interface(Message_center* message_center, Client_map* client_map, cup::Cup* cup,
                                                         const resources::Resource_store* resource_store)
: impl_(std::make_unique<Impl>(message_center, client_map, cup, resource_store))
{
}

ts::server::Interaction_interface::~Interaction_interface()
{
}

ts::server::Interaction_interface::Impl::Impl(Message_center* message_center, Client_map* client_map, cup::Cup* cup,
                                              const resources::Resource_store* resource_store)
: Message_listener(message_center),
  client_map_(client_map),
  message_center_(message_center),
  cup_(cup),
  resource_store_(resource_store),
  download_server_(message_center, resource_store)
{
    cup->add_cup_listener(this);
}

ts::server::Interaction_interface::Impl::~Impl()
{
    cup_->remove_cup_listener(this);
}

void ts::server::Interaction_interface::Impl::poll()
{
    download_server_.poll();
}

void ts::server::Interaction_interface::Impl::disconnect_client(const Generic_client& client)
{
    for (auto listener : listeners_)
    {
        listener->on_client_disconnect(client);
    }

    client_map_->disconnect_client(client);
}

void ts::server::Interaction_interface::Impl::handle_message(const Client_message& client_message)
{
    const auto& message = client_message.message;
    const auto& client = client_message.client;

    using Msg = cup::Message_type;

    if (client_message.message_type == Message_type::Reliable)
    {
        if (!client_map_->client_exists(client))
        {
            if (message.type() != Msg::registration_request)
            {
                handle_bad_registration_request(client_message);
            }

            else
            {
                handle_registration_request(client_message);
            }
        }

        else
        {
            switch (message.type())
            {
            case Msg::chat_message:
                handle_chat_message(client_message);
                break;

            case Msg::client_quit:
                handle_quit_message(client_message);
                break;

            case Msg::ready_signal:
                handle_ready_signal(client_message);
                break;

            case Msg::advance_request:
                handle_advance_request(client_message);
                break;

            case Msg::car_selection:
                handle_car_selection(client_message);
                break;
            }
        }
    }
}

void ts::server::Interaction_interface::Impl::handle_registration_request(const Client_message& client_message)
{
    const auto& client = client_message.client;
    const auto& message = client_message.message;
    auto registration_request = cup::parse_registration_request_message(message);
    auto registration_key = registration_request.registration_key;

    Client_message out_message;
    out_message.client = client;

    if (cup_->player_count() + registration_request.players.size() > 20)
    {
        // Too many players.
        out_message.message = cup::make_too_many_players_message(registration_key);
        message_center_->dispatch_message(out_message);

        disconnect_client(client);
    }
    
    else
    {
        auto remote_players = cup_->player_list();
        client_map_->register_client(client);

        for (auto& player_def : registration_request.players)
        {
            auto player_handle = client_map_->register_player(client, player_def);
            player_def.handle = player_handle->handle;

            // Send a nice chat message about this player
            cup::Composite_message display_message;
            display_message.append(player_def.nickname, sf::Color(200, 250, 0));
            display_message.append(" has joined the game, ", sf::Color(0, 220, 0));

            utf8_string player_count = std::to_string(cup_->player_count());
            player_count += "/";
            player_count += std::to_string(20);
            display_message.append(std::move(player_count), sf::Color(200, 250, 0));
            display_message.append(" players.", sf::Color(0, 220, 0));

            broadcast_chat_message(display_message);
        }

        // Send acknowledgement message back
        out_message.message = cup::make_registration_acknowledgement_message(registration_key);
        message_center_->dispatch_message(out_message);

        // And various messages about the cup
        out_message.message = cup::make_player_information_message(registration_request.players, remote_players);
        message_center_->dispatch_message(out_message);

        out_message.message = cup::make_track_information_message(resource_store_->track_settings(), resource_store_->track_store());
        message_center_->dispatch_message(out_message);

        out_message.message = cup::make_car_information_message(resource_store_->car_settings(), resource_store_->car_store());
        message_center_->dispatch_message(out_message);

        out_message.message = cup::make_cup_state_message(cup_->cup_state());
        message_center_->dispatch_message(out_message);

        resources::Track_identifier track_identifier;
        track_identifier.track_name = cup_->current_track().name();

        out_message.message = cup::make_cup_progress_message(cup_->cup_progress(), track_identifier);
        message_center_->dispatch_message(out_message);

        // Inform the listeners about the new player
        for (auto listener : listeners_)
        {
            listener->on_client_connect(client);
        }
    }
}

void ts::server::Interaction_interface::Impl::handle_bad_registration_request(const Client_message& client_message)
{
    Client_message out_message;
    out_message.client = client_message.client;
    out_message.message = cup::make_bad_request_message();

    message_center_->dispatch_message(out_message);
    disconnect_client(client_message.client);
}

void ts::server::Interaction_interface::Impl::handle_advance_request(const Client_message& client_message)
{
    const auto& client = client_message.client;
    // Only local clients may directly advance the cup.

    if (client.type() == Generic_client::Local)
    {
        cup_->advance();
    }
}

void ts::server::Interaction_interface::Impl::handle_quit_message(const Client_message& client_message)
{
    const auto& client = client_message.client;
    if (client.type() == Generic_client::Local)
    {
        Client_message out;
        out.message = cup::make_server_quit_message();
        out.message_type = Message_type::Reliable;
        message_center_->dispatch_message(out);
    }

    disconnect_client(client_message.client);
}

void ts::server::Interaction_interface::Impl::handle_chat_message(const Client_message& client_message)
{
    auto message_definition = cup::parse_chat_message(client_message.message);    
    auto displayed_message = cup::format_chat_message(client_name(client_message.client), message_definition.message);

    broadcast_chat_message(displayed_message);
}

void ts::server::Interaction_interface::Impl::broadcast_chat_message(const cup::Composite_message& displayed_message)
{
    Client_message out_message;
    out_message.message = cup::make_chatbox_output_message(displayed_message);
    message_center_->dispatch_message(out_message);
}

void ts::server::Interaction_interface::Impl::advance_if_ready()
{
    // Check if we're not waiting for any clients
    if (awaiting_clients_.empty())
    {
        cup_->advance();
    }
}

void ts::server::Interaction_interface::Impl::wait_for_everyone()
{
    awaiting_clients_.clear();

    // Add all clients to the waiting room.
    for (const auto& client : client_map_->client_list())
    {
        awaiting_clients_.insert(client);
    }
}

void ts::server::Interaction_interface::Impl::on_state_change(cup::Cup_state old_state, cup::Cup_state new_state)
{
    using cup::Cup_state;

    Client_message out_message;
    out_message.message_type = Message_type::Reliable;
    out_message.message = make_cup_state_message(new_state);
    message_center_->dispatch_message(out_message);

    if (new_state == Cup_state::Car_selection)
    {
        wait_for_everyone();
    }

    if (new_state == Cup_state::Initializing)
    {
        wait_for_everyone();
    }

    if (new_state == Cup_state::Cup)
    {
        resources::Track_identifier track_identifier;
        track_identifier.track_name = cup_->current_track().name();
        track_identifier.track_hash.fill(0);
        out_message.message = cup::make_cup_progress_message(cup_->cup_progress(), track_identifier);

        message_center_->dispatch_message(out_message);
    }
}

void ts::server::Interaction_interface::Impl::on_initialize(const cup::Stage_data& stage_data)
{
    Client_message out_message;
    out_message.message = cup::make_action_initialization_message(stage_data);
    out_message.message_type = Message_type::Reliable;
    
    message_center_->dispatch_message(out_message);
}

void ts::server::Interaction_interface::Impl::handle_ready_signal(const Client_message& message)
{
    // We're not waiting for this particular client anymore
    awaiting_clients_.erase(message.client);

    // So we can advance if everybody else if ready.
    advance_if_ready();
}

void ts::server::Interaction_interface::Impl::handle_car_selection(const Client_message& message)
{
    auto client = message.client;
    auto car_selection_message = cup::parse_car_selection_message(message.message);

    const auto& possible_cars = cup_->car_list();
    auto client_players = client_map_->get_players_by_client(client);

    // We have to be in the car selection state for this message to be accepted.
    if (cup_->cup_state() == cup::Cup_state::Car_selection)
    {
        for (const auto& entry : car_selection_message.car_selection)
        {
            auto handle = entry.player_handle;

            // Find the corresponding player in the players associated with the client
            auto search_result = std::find_if(client_players.begin(), client_players.end(),
                                              [handle](const cup::Player_handle& player_handle)
            {
                return player_handle->handle == handle;
            });

            if (search_result != client_players.end() && entry.car_id < possible_cars.size())
            {
                // Set the player's car to the car corresponding to the ID they sent us.
                const auto& selected_car = possible_cars[entry.car_id];
                const auto& player = *search_result;

                cup_->set_player_car(player, selected_car);
            }
        }
    }
}


ts::utf8_string ts::server::Interaction_interface::Impl::client_name(const Generic_client& client) const
{
    auto players = client_map_->get_players_by_client(client);
    if (players.begin() != players.end())
    {
        return (*players.begin())->nickname;
    }

    else
    {
        switch (client.type())
        {
        case Generic_client::Handle:
            return client.remote_handle().remote_address().toString();

        case Generic_client::Local:
            return "Server Admin";
        }
    }

    return utf8_string();
}

void ts::server::Interaction_interface::Impl::add_interaction_listener(Interaction_listener* listener)
{
    listeners_.push_back(listener);
}

void ts::server::Interaction_interface::Impl::remove_interaction_listener(Interaction_listener* listener)
{
    listeners_.erase(std::remove(listeners_.begin(), listeners_.end(), listener), listeners_.end());
}

void ts::server::Interaction_interface::add_interaction_listener(Interaction_listener* listener)
{
    impl_->add_interaction_listener(listener);
}

void ts::server::Interaction_interface::remove_interaction_listener(Interaction_listener* listener)
{
    impl_->remove_interaction_listener(listener);
}

void ts::server::Interaction_interface::poll()
{
    impl_->poll();
}