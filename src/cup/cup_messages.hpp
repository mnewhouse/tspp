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

#include "resources/settings/player_settings.hpp"

#include "cup.hpp"

#include "network/message.hpp"

namespace ts
{
    namespace cup
    {
        using network::Message;

        struct Message_type
        {
            static const std::uint32_t join_request = 2201;
            static const std::uint32_t join_acknowledgement = 2211;

            static const std::uint32_t bad_request = 2221;
            static const std::uint32_t too_many_players = 2222;
            static const std::uint32_t version_mismatch = 2223;

            static const std::uint32_t cup_state = 2301;
            static const std::uint32_t cup_progress = 2302;

            static const std::uint32_t player_information = 2441;
            static const std::uint32_t track_information = 2401;
            static const std::uint32_t resource_information = 2411;
            static const std::uint32_t car_information = 2421;

        };

        struct Join_request
        {
            std::uint32_t message_type = 0;
            std::uint64_t registration_key = 0;
            std::vector<cup::Player> players;
        };

        struct Join_acknowledgement
        {
            std::uint32_t message_type = 0;
            std::uint64_t registration_key = 0;
            std::uint32_t client_key = 0;
        };

        struct Join_refusal
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
        };

        struct Player_information_message
        {
            std::uint32_t message_type = 0;

            struct Player_definition
                : public Player
            {
                Player_id handle;
            };
            std::vector<Player> players;
        };

        Message make_join_request_message(std::uint64_t join_key, const resources::Player_settings& player_settings, const resources::Player_store& player_store);
        Join_request parse_join_request_message(const Message& message);

        Message make_join_acknowledgement_message(std::uint64_t registration_key, std::uint32_t client_key);
        Join_acknowledgement parse_join_acknowledgement_message(const Message& message);

        Message make_too_many_players_message(std::uint64_t registration_key);
        Message make_bad_request_message(std::uint64_t registration_key);
        Message make_version_mismatch_message(std::uint64_t registration_key);
        Join_refusal parse_join_refusal_message(const Message& message);
        
        Message make_cup_state_message(cup::Cup_state cup_state);
        Cup_state_message parse_cup_state_message(const Message& message);

        Message make_cup_progress_message(std::size_t progress);
        Cup_progress_message parse_cup_progress_message(const Message& message);

        Message make_player_information_message(const std::vector<Player_handle>& local_players, const std::vector<Player_handle>& remote_players);
        Player_information_message parse_player_information_message(const Message& message);

        Message make_track_information_message(const resources::Track_handle& track_handle);
        Message make_car_information_message(const resources::Car_settings& car_settings, const resources::Car_store& car_store);

        Message make_initialization_message();
        Message make_ready_signal_message();
    }
}

#endif