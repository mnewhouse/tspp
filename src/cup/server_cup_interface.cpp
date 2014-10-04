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

void ts::cup::Server_cup_interface::handle_join_request(const network::Client_message& client_message)
{
    const auto& client = client_message.client;
    const auto& message = client_message.message;
    auto join_request = parse_join_request_message(message);

    network::Client_message out_message;
    out_message.client = client;

    // Is there space for the new players?
    if (cup()->player_count() + join_request.players.size() > cup()->max_players())
    {
        // Too many players. Send a fail message back.
        out_message.message = make_too_many_players_message(join_request.registration_key);

        server_->send_message(out_message);
        server_->close_client_connection(client);
    }

    else
    {
        auto remote_players = cup()->player_list();
        std::vector<Player_handle> local_players;

        // Send acknowledgement message back, and add the players to the cup.
        for (auto& player : join_request.players)
        {
            auto player_handle = add_player(player);

            local_players.push_back(player_handle);
            client_player_mapping_.emplace(client->client_key(), player_handle);            
        }

        out_message.message = make_join_acknowledgement_message(join_request.registration_key, client->client_key());
        server_->send_message(out_message);

        // Also send various messages describing our lovely cup.
        out_message.message = make_cup_state_message(cup()->cup_state());
        server_->send_message(out_message);

        out_message.message = make_cup_progress_message(cup()->cup_progress(), cup()->current_track());
        server_->send_message(out_message);

        out_message.message = make_player_information_message(local_players, remote_players);
        server_->send_message(out_message);

        out_message.message = make_track_information_message(resource_store_->track_settings(), resource_store_->track_store());
        server_->send_message(out_message);

        out_message.message = make_car_information_message(resource_store_->car_settings(), resource_store_->car_store());
        server_->send_message(out_message);
    }    
}

void ts::cup::Server_cup_interface::handle_bad_join_request(network::Client_handle client)
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
}

void ts::cup::Server_cup_interface::advance_if_ready()
{
    if (awaiting_clients_.empty() && !awaiting_self_)
    {
        advance();
    }
}

void ts::cup::Server_cup_interface::wait_for_everyone()
{
    awaiting_clients_.clear();

    for (auto& client_player : client_player_mapping_)
    {
        awaiting_clients_.insert(client_player.first);
    }

    awaiting_self_ = true;
}

void ts::cup::Server_cup_interface::on_state_change(Cup_state old_state, Cup_state new_state)
{
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

        if (type != Message_type::join_request)
        {
            // ... but it isn't.
            handle_bad_join_request(client);
        }

        else
        {
            handle_join_request(client_message);
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
        }
    }
}

void ts::cup::Server_cup_interface::handle_ready_signal(const network::Client_message& message)
{
    awaiting_clients_.erase(message.client->client_key());

    advance_if_ready();
}

void ts::cup::Server_cup_interface::handle_car_selection(const network::Client_message& message)
{
    // We have to be in the car selection state for this message to be accepted.
    if (cup()->cup_state() != Cup_state::Car_selection)
    {
        return;
    }

    auto client = message.client;
    auto car_selection_message = parse_car_selection_message(message.message);

    const auto& possible_cars = cup()->car_list();

    for (const auto& entry : car_selection_message.car_selection)
    {
        auto range = client_player_mapping_.equal_range(client->client_key());
        auto handle = entry.player_handle;

        // Find the corresponding player in the players associated with the client
        auto search_result = std::find_if(range.first, range.second, 
                                          [handle](const std::pair<std::uint32_t, Player_handle>& mapping)
        {
            return mapping.second->handle == handle;
        });

        if (search_result != range.second && entry.car_id < possible_cars.size())
        {
            // Set the player's car to the car corresponding to the ID they sent us.
            set_player_car(search_result->second, possible_cars[entry.car_id]);
        }
    }
}