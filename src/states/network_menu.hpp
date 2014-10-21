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

#ifndef STATES_NETWORK_MENU
#define STATES_NETWORK_MENU

#include "user_interface/document_handle.hpp"
#include "user_interface/elements/elements.hpp"

#include "client/registration.hpp"

namespace ts
{
    namespace states
    {
        class Main_menu;
        class Cup_setup_menu;
        class Client_cup_state;
        class Connection_dialog;

        class Network_menu
        {
        public:
            Network_menu(Main_menu* main_menu, Cup_setup_menu* cup_setup_menu);
            ~Network_menu();

            void show();
            void hide();

            void update(std::size_t frame_duration);

            void cancel_connect_attempt();

        private:
            void activate_cup_state();
            void handle_registration_error(const utf8_string& error_message);

            void initiate_client_connection(utf8_string address_string);

            void return_to_main_menu();
            void show_server_setup();
            void create_network_menu_document(gui::Context* context);

            Main_menu* main_menu_ = nullptr;
            Cup_setup_menu* cup_setup_menu_ = nullptr;

            gui::Edit_box* server_address_editbox_ = nullptr;
            gui::Vertical_list<gui::Text_element>* recent_servers_list_ = nullptr;

            std::unique_ptr<Client_cup_state> client_cup_state_;
            std::unique_ptr<Connection_dialog> connection_dialog_;

            client::Registration_status registration_status_ = client::Registration_status::None;

            gui::Document_handle network_menu_;
        };

        class Connection_dialog
        {
        public:
            Connection_dialog(gui::Context* context, Network_menu* network_menu);

            void show(Client_cup_state* client_cup_state);
            void hide();

            void set_status_text(utf8_string status_text);

        private:
            void cancel_connect_attempt();

            void create_connection_dialog_document(gui::Context* context);

            Client_cup_state* client_cup_state_ = nullptr;
            Network_menu* network_menu_ = nullptr;

            gui::Text_element* connection_state_text_ = nullptr;    
            gui::Text_element* cancel_button_ = nullptr;

            gui::Document_handle connection_dialog_;
        };
    }
}

#endif