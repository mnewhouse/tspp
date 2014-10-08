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

#ifndef CLIENT_CUP_INTERFACE_HPP
#define CLIENT_CUP_INTERFACE_HPP

#include "network/client.hpp"
#include "cup_interface.hpp"
#include "registration.hpp"

namespace ts
{
    namespace resources
    {
        struct Resource_store;
    }

    namespace cup
    {
        class Client_cup_interface
            : public Cup_interface
        {
        public:
            Client_cup_interface(Cup* cup, network::Client* client, const resources::Resource_store* resource_store);
            ~Client_cup_interface();

            void send_registration_request();
            void update();

            Registration_status registration_status() const;
            const utf8_string& registration_error() const;
                        
            virtual void select_cars(const std::vector<Car_selection>& car_selection) override;
            virtual void signal_ready() override;
            virtual void write_chat_message(const utf8_string& message) override;

        private:
            void registration_error(utf8_string error_string);

            void handle_message(const network::Message& message);

            void handle_too_many_players();
            void handle_invalid_response();
            void handle_version_mismatch();
            
            void handle_acknowledgement_message(const network::Message& message);
            void handle_cup_state_message(const network::Message& message);
            void handle_cup_progress_message(const network::Message& message);
            void handle_player_information_message(const network::Message& message);
            void handle_track_information_message(const network::Message& message);
            void handle_car_information_message(const network::Message& message);
            void handle_action_initialization_message(const network::Message& message);

            void handle_chatbox_output_message(const network::Message& message);

            Cup* cup_;
            network::Client* client_;
            network::Message message_buffer_;
            const resources::Resource_store* resource_store_;
            std::uint64_t registration_key_ = 0;
            std::uint32_t client_key_ = 0;

            Registration_status registration_status_ = Registration_status::None;
            utf8_string registration_error_;
        };
    }
}

#endif