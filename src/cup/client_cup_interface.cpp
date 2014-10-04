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
#include "client_cup_interface.hpp"

#include "cup_messages.hpp"

#include "network/message_reader.hpp"

#include "resources/resource_store.hpp"

ts::cup::Client_cup_interface::Client_cup_interface(Cup* cup, network::Client* client, const resources::Resource_store* resource_store)
: Cup_interface(cup),
  client_(client),
  resource_store_(resource_store),
  registration_key_(client->generate_key())
{
}

ts::cup::Client_cup_interface::~Client_cup_interface()
{
}

void ts::cup::Client_cup_interface::update()
{
    while (client_->get_message(message_buffer_))
    {
        if (client_key_ == 0)
        {
            switch (message_buffer_.type())
            {
            case Message_type::join_acknowledgement:
                handle_acknowledgement_message(message_buffer_);
                break;

            case Message_type::version_mismatch:
                handle_version_mismatch();
                break;

            case Message_type::too_many_players:
                handle_too_many_players();
                break;

            default:
                handle_invalid_response();
                break;
            }
        }

        else
        {
            handle_message(message_buffer_);
        }
    }
}

void ts::cup::Client_cup_interface::select_cars(const std::vector<Car_selection>& car_selection)
{

}

void ts::cup::Client_cup_interface::signal_ready()
{

}

void ts::cup::Client_cup_interface::write_chat_message(const utf8_string& message)
{
}

void ts::cup::Client_cup_interface::handle_invalid_response()
{
    registration_error("invalid response");
}

void ts::cup::Client_cup_interface::handle_too_many_players()
{
    registration_error("too many players");
}

void ts::cup::Client_cup_interface::handle_version_mismatch()
{
    registration_error("version mismatch");
}

void ts::cup::Client_cup_interface::handle_acknowledgement_message(const Message& message)
{
    auto join_acknowledgement = parse_join_acknowledgement_message(message);

    if (join_acknowledgement.registration_key != registration_key_)
    {
        handle_invalid_response();
    }

    else
    {
        client_key_ = join_acknowledgement.client_key;
        registration_status_ = Registration_status::Registered;
    }
}

void ts::cup::Client_cup_interface::handle_cup_state_message(const Message& message)
{
    auto cup_state_message = parse_cup_state_message(message);
    set_cup_state(cup_state_message.cup_state);
}

void ts::cup::Client_cup_interface::handle_message(const Message& message)
{
    switch (message.type())
    {
    case Message_type::cup_state:
        handle_cup_state_message(message);
        break;
    }
}

void ts::cup::Client_cup_interface::registration_error(utf8_string error_string)
{
    registration_error_ = std::move(error_string);

    client_->disconnect();
}

void ts::cup::Client_cup_interface::send_registration_request()
{
    const auto& player_store = resource_store_->player_store();
    const auto& player_settings = resource_store_->player_settings();

    auto message = make_join_request_message(registration_key_, player_settings, player_store);
    client_->send_message(std::move(message));
    
    registration_status_ = Registration_status::Registering;
}

ts::cup::Registration_status ts::cup::Client_cup_interface::registration_status() const
{
    return registration_status_;
}

const ts::utf8_string& ts::cup::Client_cup_interface::registration_error() const
{
    return registration_error_;
}