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

#pragma once

#ifndef MESSAGE_DEFINITIONS_HPP
#define MESSAGE_DEFINITIONS_HPP

#include "cup/cup_metadata.hpp"
#include "cup/chat_message.hpp"

#include "messages/message.hpp"

#include "resources/track_identifier.hpp"
#include "resources/car_identifier.hpp"
#include "resources/resource_identifier.hpp"

#include "resources/settings/player_settings.hpp"
#include "resources/settings/track_settings.hpp"
#include "resources/settings/car_settings.hpp"

namespace ts
{
    namespace resources
    {
        class Track_store;
        class Car_store;

        class Car_handle;

    }

    namespace cup
    {
        using messages::Message;

        struct Message_type
        {
            // Client -> server messages
            static const std::uint32_t registration_request = 1201;
            static const std::uint32_t car_selection = 1471;
            static const std::uint32_t ready_signal = 1481;
            static const std::uint32_t chat_message = 1691;

            static const std::uint32_t client_quit = 1771;
            static const std::uint32_t advance_request = 1779;

            // Download requests
            static const std::uint32_t track_download_request = 3401;
            static const std::uint32_t car_download_request = 3411;
            static const std::uint32_t resource_download_request = 3421;


            // Server -> client messages
            static const std::uint32_t registration_acknowledgement = 2211;

            static const std::uint32_t bad_request = 2221;
            static const std::uint32_t too_many_players = 2222;
            static const std::uint32_t version_mismatch = 2223;

            static const std::uint32_t cup_state = 2301;
            static const std::uint32_t cup_progress = 2302;
            static const std::uint32_t car_selection_initiation = 2346;
            static const std::uint32_t action_initialization = 2351;

            static const std::uint32_t player_information = 2441;
            static const std::uint32_t track_information = 2401;
            static const std::uint32_t resource_information = 2411;
            static const std::uint32_t car_information = 2421;            

            static const std::uint32_t chatbox_output = 2831;
            static const std::uint32_t server_quit = 2571;

            // Misc. messages
            static const std::uint32_t load_error = 7115;
        };

        struct Stage_data;

        struct Registration_request
        {
            std::uint32_t message_type = 0;
            std::uint64_t registration_key = 0;
            std::vector<Player_definition> players;
        };

        struct Registration_acknowledgement
        {
            std::uint32_t message_type = 0;
            std::uint64_t registration_key = 0;
        };

        struct Registration_refusal
        {
            std::uint32_t message_type = 0;
            std::uint64_t registration_key = 0;
        };

        struct Cup_state_message
        {
            std::uint32_t message_type = 0;
            cup::Cup_state cup_state = cup::Cup_state::Registering;
        };

        struct Cup_progress_message
        {
            std::uint32_t message_type = 0;
            std::size_t cup_progress = 0;
            resources::Track_identifier track_identifier;
        };

        struct Player_information_message
        {
            std::uint32_t message_type = 0;

            std::vector<Player_definition> players;
        };

        struct Track_information_message
        {
            std::uint32_t message_type = 0;
            std::vector<utf8_string> track_names;
        };

        struct Car_information_message
        {
            std::uint32_t message_type = 0;
            resources::Car_mode car_mode = resources::Car_mode::Fixed;
            std::vector<resources::Car_identifier> cars;
        };

        struct Car_selection_message
        {
            std::uint32_t message_type = 0;
            struct Entry
            {
                Player_id player_handle;
                std::uint32_t car_id;
            };

            std::vector<Entry> car_selection;
        };

        struct Chat_message_definition
        {
            std::uint32_t message_type = 0;
            utf8_string message;
        };

        struct Chatbox_output_message
        {
            std::uint32_t message_type = 0;
            Chat_message message;
        };

        struct Action_initialization_message
        {
            std::uint32_t message_type = 0;

            struct Car
            {
                std::uint16_t car_id;                
                std::uint16_t start_pos;
                std::uint16_t player;
                resources::Player_color color;
                resources::Car_identifier car_identifier;

                std::size_t resource_index = std::numeric_limits<std::size_t>::max();
            };

            resources::Track_identifier track_identifier;
            std::vector<resources::Resource_identifier> resource_list;
            std::vector<Car> car_list;            
        };

        struct Load_error_message
        {
            std::uint32_t message_type = 0;
            utf8_string error_string;
        };

        // Client -> server messages
        Message make_registration_request_message(std::uint64_t registration_key, const resources::Player_settings& player_settings, const resources::Player_store& player_store);
        Registration_request parse_registration_request_message(const Message& message);

        Message make_ready_signal_message();
        Message make_advance_request_message();
        Message make_client_quit_message();

        Message make_chat_message(const utf8_string& message);
        Chat_message_definition parse_chat_message(const Message& message);

        Message make_car_selection_message(const std::vector<Car_selection>& car_selection);
        Car_selection_message parse_car_selection_message(const Message& message);

        

        // Server -> client messages
        Message make_registration_acknowledgement_message(std::uint64_t registration_key);
        Registration_acknowledgement parse_registration_acknowledgement_message(const Message& message);

        Message make_too_many_players_message(std::uint64_t registration_key);
        Message make_version_mismatch_message(std::uint64_t registration_key);

        Message make_bad_request_message();
        Registration_refusal parse_registration_refusal_message(const Message& message);
        
        Message make_cup_state_message(cup::Cup_state cup_state);
        Cup_state_message parse_cup_state_message(const Message& message);

        Message make_cup_progress_message(std::size_t progress, const resources::Track_identifier& track_handle);
        Cup_progress_message parse_cup_progress_message(const Message& message);

        Message make_player_information_message(const std::vector<Player_definition>& local_players, const std::vector<Player_handle>& remote_players);
        Player_information_message parse_player_information_message(const Message& message);

        Message make_track_information_message(const resources::Track_settings& track_settings, const resources::Track_store& track_store);
        Track_information_message parse_track_information_message(const Message& message);

        Message make_car_information_message(const resources::Car_settings& car_settings);
        Car_information_message parse_car_information_message(const Message& message);

        Message make_car_selection_initiation_message(const resources::Car_settings& car_settings);
        Car_information_message parse_car_selection_initiation_message(const Message& message);

        Message make_chatbox_output_message(const Chat_message& chat_message);
        Chatbox_output_message parse_chatbox_output_message(const Message& message);

        Message make_action_initialization_message(const Stage_data& stage_data);
        Action_initialization_message parse_action_initialization_message(const Message& message);

        Message make_server_quit_message();

        Message make_load_error_message(const utf8_string& error_string);
        Load_error_message parse_load_error_message(const Message& message);
    }
}

#endif