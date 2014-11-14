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
#include "server_stage_interface.hpp"
#include "command_center.hpp"

#include "cup/cup.hpp"
#include "cup/cup_listener.hpp"
#include "cup/cup_controller.hpp"
#include "cup/cup_messages.hpp"
#include "cup/stage_data.hpp"
#include "cup/chatbox.hpp"


#include "resources/resource_store.hpp"

class ts::server::Interaction_interface::Impl
    : public Message_listener, public cup::Cup_listener
{
public:
    Impl(Message_center* message_center, Client_map* client_map, 
         cup::Cup_controller* cup_controller, const Stage_interface* stage_interface, const Command_center* command_center);

    ~Impl();

    void add_interaction_listener(Interaction_listener* listener);
    void remove_interaction_listener(Interaction_listener* listener);

    void broadcast_chat_message(const cup::Chat_message& message);

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

    void broadcast_cup_progress();

    void wait_for_everyone();
    void advance_if_ready();
    void handle_ready_signal(const Client_message& client_message);
    void handle_car_selection(const Client_message& client_message);

    Client_map* client_map_;
    Message_center* message_center_;
    cup::Cup_controller* cup_controller_;
    const Stage_interface* stage_interface_;
    const Command_center* command_center_;

    std::vector<Interaction_listener*> listeners_;
    std::unordered_set<Generic_client> awaiting_clients_;
};

ts::server::Interaction_interface::Interaction_interface(Message_center* message_center, Client_map* client_map, 
                                                         cup::Cup_controller* cup_controller, const Stage_interface* stage_interface,
                                                         const Command_center* command_center)
: impl_(std::make_unique<Impl>(message_center, client_map, cup_controller, stage_interface, command_center))
{
}

ts::server::Interaction_interface::~Interaction_interface()
{
}

ts::server::Interaction_interface::Impl::Impl(Message_center* message_center, Client_map* client_map, 
                                              cup::Cup_controller* cup_controller, const Stage_interface* stage_interface,
                                              const Command_center* command_center)
: Message_listener(message_center),
  client_map_(client_map),
  message_center_(message_center),
  cup_controller_(cup_controller),
  stage_interface_(stage_interface),
  command_center_(command_center)
{
    cup_controller_->add_cup_listener(this);
}

ts::server::Interaction_interface::Impl::~Impl()
{
    cup_controller_->remove_cup_listener(this);
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

    if (cup_controller_->cup()->player_count() + registration_request.players.size() > 20)
    {
        // Too many players.
        out_message.message = cup::make_too_many_players_message(registration_key);
        message_center_->dispatch_message(out_message);

        disconnect_client(client);
    }
    
    else
    {
        auto remote_players = cup_controller_->cup()->player_list();
        client_map_->register_client(client);

        for (auto& player_def : registration_request.players)
        {
            auto player_handle = client_map_->register_player(client, player_def);
            player_def.handle = player_handle->handle;

            // Send a nice chat message about this player
            cup::Chat_message display_message;
            display_message.append(player_def.nickname, sf::Color(200, 250, 0));
            display_message.append(" has joined the game, ", sf::Color(0, 220, 0));

            utf8_string player_count = std::to_string(cup_controller_->cup()->player_count());
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

        out_message.message = cup::make_car_information_message(cup_controller_->car_settings());
        message_center_->dispatch_message(out_message);

        out_message.message = cup::make_cup_state_message(cup_controller_->cup_state());
        message_center_->dispatch_message(out_message);

        auto cup_state = cup_controller_->cup_state();
        if (cup_state != cup::Cup_state::Registering)
        {
            resources::Track_identifier track_identifier;
            track_identifier.track_name = cup_controller_->current_track().name();

            out_message.message = cup::make_cup_progress_message(cup_controller_->cup_progress(), track_identifier);
            message_center_->dispatch_message(out_message);
        }

        if (cup_state == cup::Cup_state::Initializing || cup_state == cup::Cup_state::Action)
        {
            // Send action initialization message.
            out_message.message = cup::make_action_initialization_message(stage_interface_->stage_data());
            message_center_->dispatch_message(out_message);
        }

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
        cup_controller_->advance();
    }
}

void ts::server::Interaction_interface::Impl::handle_quit_message(const Client_message& client_message)
{
    const auto& client = client_message.client;
    if (client.type() == Generic_client::Local)
    {
        // Local client requested quit.
        // That means the whole server is quitting.
        // We should inform all clients of this.

        Client_message out;
        out.message = cup::make_server_quit_message();

        message_center_->dispatch_message(out);
    }

    else
    {
        cup::Chat_message displayed_message;
        displayed_message.append(client_map_->client_name(client), sf::Color(200, 250, 0));
        displayed_message.append(" has left the game.", sf::Color(0, 220, 0));

        Client_message out;
        out.message = cup::make_chatbox_output_message(displayed_message);
        message_center_->dispatch_message(out);

        disconnect_client(client_message.client);
    }    
}

void ts::server::Interaction_interface::Impl::handle_chat_message(const Client_message& client_message)
{
    auto message_definition = cup::parse_chat_message(client_message.message);    
    const auto& message = message_definition.message;
    if (!command_center_->has_command_prefix(message))
    {
        auto displayed_message = cup::format_chat_message(client_map_->client_name(client_message.client), message);
        broadcast_chat_message(displayed_message);
    }

    else
    {
        command_center_->handle_client_message(client_message.client, message);
    }
}

void ts::server::Interaction_interface::Impl::broadcast_chat_message(const cup::Chat_message& displayed_message)
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
        cup_controller_->advance();
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
    out_message.message = make_cup_state_message(new_state);
    message_center_->dispatch_message(out_message);

    if (old_state == Cup_state::Car_selection || old_state == Cup_state::Initializing)
    {
        // No need to wait for anyone anymore
        awaiting_clients_.clear();
    }

    if (new_state == Cup_state::Car_selection)
    {
        out_message.message = cup::make_car_selection_initiation_message(cup_controller_->car_settings());
        message_center_->dispatch_message(out_message);

        wait_for_everyone();
    }

    else if (new_state == Cup_state::Initializing)
    {
        wait_for_everyone();
    }

    else if (new_state == Cup_state::Cup)
    {
        broadcast_cup_progress();
    }
}

void ts::server::Interaction_interface::Impl::broadcast_cup_progress()
{
    
    resources::Track_identifier track_identifier;
    track_identifier.track_name = cup_controller_->current_track().name();
    track_identifier.track_hash.fill(0);
        
    auto progress = cup_controller_->cup_progress();
    auto track_count = cup_controller_->stage_count();

    Client_message out_message;
    out_message.message = cup::make_cup_progress_message(progress, track_identifier);
    message_center_->dispatch_message(out_message);

    cup::Chat_message displayed_message;
    displayed_message.append("Track ", sf::Color(163, 218, 255));

    utf8_string progress_string = std::to_string(progress + 1);
    progress_string += "/";
    progress_string += std::to_string(track_count);
    displayed_message.append(std::move(progress_string), sf::Color(138, 249, 255));
    displayed_message.append(" will be ", sf::Color(163, 218, 255));
    displayed_message.append(track_identifier.track_name, sf::Color(138, 249, 255));
    displayed_message.append(".", sf::Color(163, 218, 255));

    out_message.message = cup::make_chatbox_output_message(displayed_message);
    message_center_->dispatch_message(out_message);
}

void ts::server::Interaction_interface::Impl::on_initialize(const cup::Stage_data& stage_data)
{
    Client_message out_message;
    out_message.message = cup::make_action_initialization_message(stage_data);
    
    message_center_->dispatch_message(out_message);
}

void ts::server::Interaction_interface::Impl::handle_ready_signal(const Client_message& message)
{
    auto cup_state = cup_controller_->cup_state();
    if (cup_state == cup::Cup_state::Car_selection || cup_state == cup::Cup_state::Initializing)
    {        
        // We're not waiting for this particular client anymore
        awaiting_clients_.erase(message.client);

        cup::Chat_message displayed_message;
        displayed_message.append(client_map_->client_name(message.client), sf::Color(255, 220, 50));
        displayed_message.append(" ready for action.", sf::Color(255, 150, 0));

        Client_message out_message;
        out_message.message = cup::make_chatbox_output_message(displayed_message);
        message_center_->dispatch_message(out_message);

        // So we can advance if everybody else if ready.
        advance_if_ready();
    }
}

void ts::server::Interaction_interface::Impl::handle_car_selection(const Client_message& message)
{
    auto client = message.client;
    auto car_selection_message = cup::parse_car_selection_message(message.message);

    const auto& possible_cars = cup_controller_->car_list();
    auto client_players = client_map_->get_players_by_client(client);

    // We have to be in the car selection state for this message to be accepted.
    if (cup_controller_->cup_state() == cup::Cup_state::Car_selection)
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

                cup_controller_->set_player_car(player, selected_car);
            }
        }

        awaiting_clients_.erase(client);
        advance_if_ready();
    }
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
