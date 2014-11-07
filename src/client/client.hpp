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

#ifndef CLIENT_CLIENT_HPP
#define CLIENT_CLIENT_HPP

#include "registration.hpp"

#include "network/client_connection.hpp"


namespace ts
{
    namespace cup
    {
        class Cup;
        struct Cup_listener;

        struct Chatbox_listener;
        class Chatbox;

        struct Stage_data;
    }

    namespace controls
    {
        class Control_interface;
    }

    namespace resources
    {
        struct Resource_store;
    }

    namespace action
    {
        class Stage;
    }

    namespace game
    {
        class Stage_loader;
    }

    namespace client
    {
        namespace impl
        {
            class Client;
        }

        using network::Connection_status;

        class Client_interface;

        class Client
        {
        public:
            Client(resources::Resource_store* resource_store);
            ~Client();

            const Client_interface* client_interface() const;
            controls::Control_interface* make_control_interface();

            Connection_status connection_status() const;
            Registration_status registration_status() const;

            const utf8_string& registration_error() const;

            void add_cup_listener(cup::Cup_listener* listener);
            void remove_cup_listener(cup::Cup_listener* listener);

            void add_chatbox_listener(cup::Chatbox_listener* listener);
            void remove_chatbox_listener(cup::Chatbox_listener* listener);

            const cup::Chatbox* chatbox() const;

            void async_connect(utf8_string remote_address, std::uint16_t remote_port);
            void send_registration_request();
            
            bool is_downloading() const;
            std::pair<std::size_t, std::size_t> download_progress() const;

            void update(std::size_t frame_duration);

            void launch_action();
            void end_action();
            const action::Stage* stage() const;

            const game::Stage_loader* async_load_stage(const cup::Stage_data& stage_data, std::function<void(const action::Stage*)> completion_callback);

        private:
            std::unique_ptr<impl::Client> impl_;
        };
    }
}

#endif