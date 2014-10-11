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
#include "cup.hpp"
#include "cup_messages.hpp"
#include "stage_data.hpp"

#include "messages/message_reader.hpp"

#include "resources/resource_store.hpp"
#include "resources/track_store.hpp"
#include "resources/car_store.hpp"

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

bool ts::cup::Client_cup_interface::get_message(messages::Message& message) const
{
    return client_->get_message(message);
}

void ts::cup::Client_cup_interface::update(std::size_t frame_duration, std::function<void(const messages::Message& message)> message_callback)
{
    while (get_message(message_buffer_))
    {
        handle_message(message_buffer_);

        if (message_callback)
        {
            message_callback(message_buffer_);
        }
    }
}

void ts::cup::Client_cup_interface::select_cars(const std::vector<Car_selection>& car_selection)
{
    auto message = make_car_selection_message(car_selection);
    client_->send_message(message);
}

void ts::cup::Client_cup_interface::signal_ready()
{
    auto message = make_ready_signal_message();
    client_->send_message(message);
}

void ts::cup::Client_cup_interface::write_chat_message(const utf8_string& text)
{
    auto message = make_chat_message(text);
    client_->send_message(message);
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
    auto join_acknowledgement = parse_registration_acknowledgement_message(message);

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

void ts::cup::Client_cup_interface::handle_cup_progress_message(const Message& message)
{
    auto cup_progress_message = parse_cup_progress_message(message);
    set_cup_progress(cup_progress_message.cup_progress);
}

void ts::cup::Client_cup_interface::handle_player_information_message(const Message& message)
{
    auto player_information = parse_player_information_message(message);
    for (auto& player : player_information.players)
    {
        add_player(player, player.handle);
    }
}

void ts::cup::Client_cup_interface::handle_track_information_message(const Message& message)
{
    const auto& track_store = resource_store_->track_store();

    clear_tracks();

    auto track_information = parse_track_information_message(message);
    for (const auto& track_name : track_information.track_names)
    {
        auto track_handle = track_store.get_track_by_name(track_name);
        add_track(track_handle);
    }
}

void ts::cup::Client_cup_interface::handle_car_information_message(const Message& message)
{
    const auto& car_store = resource_store_->car_store();
    auto car_information = parse_car_information_message(message);

    set_car_mode(car_information.car_mode);

    for (const auto& car_name : car_information.car_names)
    {
        auto car_handle = car_store.get_car_by_name(car_name);

        select_car(car_handle);
    }
}

void ts::cup::Client_cup_interface::handle_action_initialization_message(const Message& message)
{
    auto action_data = parse_action_initialization_message(message);

    const auto& car_store = resource_store_->car_store();
    const auto& track_store = resource_store_->track_store();

    Stage_data stage_data;
    for (const auto& car : action_data.car_list)
    {
        if (auto car_def = car_store.get_car_by_name(car.car_name))
        {
            Car_data car_data;
            car_data.car_def = car_def;
            car_data.car_id = car.car_id;
            car_data.player.color = car.color;
            car_data.start_pos = car.start_pos;
            car_data.controller = cup()->get_player_by_id(car.player);

            stage_data.cars.push_back(car_data);
        }
    }

    // TODO: Handle resource downloading
    stage_data.track = track_store.get_track_by_name(action_data.track_name);

    initialize_action(stage_data);
}

void ts::cup::Client_cup_interface::handle_message(const Message& message)
{    
    if (client_key_ == 0)
    {
        switch (message_buffer_.type())
        {
        case Message_type::registration_acknowledgement:
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
        switch (message.type())
        {
        case Message_type::cup_state:
            handle_cup_state_message(message);
            break;

        case Message_type::player_information:
            handle_player_information_message(message);
            break;

        case Message_type::track_information:
            handle_track_information_message(message);
            break;

        case Message_type::car_information:
            handle_car_information_message(message);
            break;

        case Message_type::cup_progress:
            handle_cup_progress_message(message);
            break;

        case Message_type::chatbox_output:
            handle_chatbox_output_message(message);
            break;

        case Message_type::action_initialization:
            handle_action_initialization_message(message);
            break;
        }
    }
}

void ts::cup::Client_cup_interface::handle_chatbox_output_message(const Message& message)
{
    auto message_definition = parse_chatbox_output_message(message);
    output_chat_message(message_definition.message);
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

    auto message = make_registration_request_message(registration_key_, player_settings, player_store);
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