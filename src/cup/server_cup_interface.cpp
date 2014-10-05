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
#include "server_cup_interface.hpp"
#include "cup.hpp"
#include "cup_messages.hpp"

#include "network/server.hpp"
#include "network/message_reader.hpp"

#include "resources/resource_store.hpp"

ts::cup::Server_cup_interface::Server_cup_interface(Cup* cup, network::Server* server, const resources::Resource_store* resource_store)
: Cup_interface(cup),
  server_(server),
  resource_store_(resource_store)
{
}

void ts::cup::Server_cup_interface::handle_registration_request(const network::Client_message& client_message)
{
    const auto& client = client_message.client;
    const auto& message = client_message.message;
    auto registration_request = parse_registration_request_message(message);

    network::Client_message out_message;
    out_message.client = client;

    // Is there space for the new players?
    if (cup()->player_count() + registration_request.players.size() > cup()->max_players())
    {
        // Too many players. Send a fail message back.
        out_message.message = make_too_many_players_message(registration_request.registration_key);

        server_->send_message(out_message);
        server_->close_client_connection(client);
    }

    else
    {
        auto remote_players = cup()->player_list();

        auto& player_list = client_player_mapping_[client->client_key()];
        
        // Send acknowledgement message back
        out_message.message = make_registration_acknowledgement_message(registration_request.registration_key, client->client_key());
        server_->send_message(out_message);

        
        for (auto& player : registration_request.players)
        {
            auto slot = player.control_slot;

            // Set the control slot to invalid so that the server admin won't 
            // be able to control the client's car
            player.control_slot = controls::invalid_slot;

            auto player_handle = add_player(player);
            player_list.push_back(player_handle);

            // Assign the unique handle we created to the player
            player.handle = player_handle->handle;

            // And revert the control slot
            player.control_slot = slot;            
        }

        // Also send various messages describing our lovely cup.
        out_message.message = make_cup_state_message(cup()->cup_state());
        server_->send_message(out_message);

        out_message.message = make_cup_progress_message(cup()->cup_progress(), cup()->current_track());
        server_->send_message(out_message);

        out_message.message = make_track_information_message(resource_store_->track_settings(), resource_store_->track_store());
        server_->send_message(out_message);

        out_message.message = make_car_information_message(resource_store_->car_settings(), resource_store_->car_store());
        server_->send_message(out_message);

        out_message.message = make_player_information_message(registration_request.players, remote_players);
        server_->send_message(out_message);
    }    
}

void ts::cup::Server_cup_interface::handle_bad_registration_request(network::Client_handle client)
{
    network::Client_message outgoing_message;
    outgoing_message.client = client;
    outgoing_message.message = make_bad_request_message(client->client_key());

    server_->send_message(outgoing_message);
    server_->close_client_connection(client);
}

void ts::cup::Server_cup_interface::disconnect_client(network::Client_handle client)
{
    awaiting_clients_.erase(client->client_key());
    client_player_mapping_.erase(client->client_key());

    server_->close_client_connection(client);
}

void ts::cup::Server_cup_interface::update(std::size_t frame_duration)
{
    while (server_->get_message(message_buffer_))
    {
        handle_message(message_buffer_);
    }
}

void ts::cup::Server_cup_interface::select_cars(const std::vector<Car_selection>& car_selection)
{
    // Local player selected a car.
    for (auto& selected_car : car_selection)
    {
        set_player_car(selected_car.player_handle, selected_car.car_handle);
    }

    awaiting_self_ = false;
    advance_if_ready();
}

void ts::cup::Server_cup_interface::signal_ready()
{
    awaiting_self_ = false;
    advance_if_ready();
}

void ts::cup::Server_cup_interface::write_chat_message(const utf8_string& message)
{
    auto displayed_message = format_chat_message(my_name(), message);
    output_chat_message(displayed_message);

    auto outgoing_message = make_chatbox_output_message(displayed_message);
    server_->send_message_to_all(outgoing_message);
}

void ts::cup::Server_cup_interface::advance_if_ready()
{
    // Check if we're not waiting for any clients or ourselves
    if (awaiting_clients_.empty() && !awaiting_self_)
    {
        advance();
    }
}

void ts::cup::Server_cup_interface::wait_for_everyone()
{
    awaiting_clients_.clear();

    // Add all clients to the waiting room.
    for (auto& client_player : client_player_mapping_)
    {
        awaiting_clients_.insert(client_player.first);
    }

    // And ourselves, too.
    awaiting_self_ = true;
}

void ts::cup::Server_cup_interface::on_state_change(Cup_state old_state, Cup_state new_state)
{
    auto message = make_cup_state_message(new_state);
    server_->send_message_to_all(message);

    if (new_state == Cup_state::Car_selection)
    {
        wait_for_everyone();
    }

    if (new_state == Cup_state::Initializing)
    {
        wait_for_everyone();
    }
}

void ts::cup::Server_cup_interface::handle_message(const network::Client_message& client_message)
{    
    const auto& client = client_message.client;
    const auto& message = client_message.message;

    network::Message_reader message_reader(message);

    std::uint32_t type = 0;
    message_reader >> type;

    if (client_player_mapping_.count(client->client_key()) == 0)
    {
        // The first message a client sends has to be a join request.

        if (type != Message_type::registration_request)
        {
            // ... but it isn't.
            handle_bad_registration_request(client);
        }

        else
        {
            // Seems legit.
            handle_registration_request(client_message);
        }
    }

    else
    {
        switch (type)
        {
        case Message_type::ready_signal:
            handle_ready_signal(client_message);
            break;

        case Message_type::car_selection:
            handle_car_selection(client_message);
            break;

        case Message_type::chat_message:
            handle_chat_message(client_message);
            break;
        }
    }
}

ts::utf8_string ts::cup::Server_cup_interface::client_name(network::Client_handle client) const
{
    auto mapping_it = client_player_mapping_.find(client->client_key());
    if (mapping_it != client_player_mapping_.end())
    {
        const auto& player_list = mapping_it->second;
        if (!player_list.empty())
        {
            return player_list.front()->nickname;
        }
    }

    return client->remote_address().toString();
}

ts::utf8_string ts::cup::Server_cup_interface::my_name() const
{
    const auto& local_players = cup()->local_players();
    if (!local_players.empty())
    {
        return local_players.front()->nickname;
    }

    return "Server Admin";
}

void ts::cup::Server_cup_interface::handle_chat_message(const network::Client_message& message)
{
    // Client sent a chat message.
    // Output it and distribute it to everybody else.
    
    auto message_definition = parse_chat_message(message.message);

    auto displayed_message = format_chat_message(client_name(message.client), message_definition.message);
    output_chat_message(displayed_message);

    auto outgoing_message = make_chatbox_output_message(displayed_message);
    server_->send_message_to_all(outgoing_message);
}

void ts::cup::Server_cup_interface::handle_ready_signal(const network::Client_message& message)
{
    // We're not waiting for this particular client anymore
    awaiting_clients_.erase(message.client->client_key());

    // So we can advance if everybody else if ready.
    advance_if_ready();
}

void ts::cup::Server_cup_interface::handle_car_selection(const network::Client_message& message)
{
    auto client = message.client;
    auto car_selection_message = parse_car_selection_message(message.message);

    const auto& possible_cars = cup()->car_list();
    auto client_it = client_player_mapping_.find(client->client_key());

    // We have to be in the car selection state for this message to be accepted.
    if (cup()->cup_state() != Cup_state::Car_selection && client_it != client_player_mapping_.end())
    {
        const auto& player_list = client_it->second;

        for (const auto& entry : car_selection_message.car_selection)
        {
            auto handle = entry.player_handle;

            // Find the corresponding player in the players associated with the client
            auto search_result = std::find_if(player_list.begin(), player_list.end(),
                                              [handle](const Player_handle& player_handle)
            {
                return player_handle->handle == handle;
            });

            if (search_result != player_list.end() && entry.car_id < possible_cars.size())
            {
                // Set the player's car to the car corresponding to the ID they sent us.
                set_player_car(*search_result, possible_cars[entry.car_id]);
            }
        }
    }
}