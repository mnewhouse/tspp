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

#include "network/server.hpp"

#include "cup_messages.hpp"

#include "network/message_reader.hpp"

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
        // Send acknowledgement message back, and add the players to the cup.
        for (auto& player : join_request.players)
        {
            auto player_handle = add_player(player);
            client_player_mapping_.emplace(client->client_key(), player_handle);            
        }

        out_message.message = make_join_acknowledgement_message(join_request.registration_key, client->client_key());
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
        const auto& client = message_buffer_.client;
        const auto& message = message_buffer_.message;

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
                handle_join_request(message_buffer_);
            }
        }
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