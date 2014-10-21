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

#ifndef SERVER_SERVER_HPP
#define SERVER_SERVER_HPP

#include "server_messages.hpp"

#include "network/client_handle.hpp"

namespace ts
{
    namespace cup
    {
        class Cup;
        struct Cup_listener;
        struct Stage_data;
        struct Player_definition;
    }

    namespace action
    {
        class Stage;
    }

    namespace game
    {
        class Stage_loader;
    }

    namespace resources
    {
        struct Resource_store;
    }

    namespace server
    {
        namespace impl
        {
            class Server;
        }

        class Server
        {
        public:
            Server(resources::Resource_store* resource_store);
            ~Server();

            void poll();
            void update_stage(std::size_t frame_duration);

            void listen(std::uint16_t listen_port);

            using Local_client_link = std::function<void(const Client_message&)>;

            void register_local_client(Local_client_link local_client_link, 
                                       const std::vector<cup::Player_definition>& player_def);
            void unlink_local_client();

            const Message_center* message_center() const;
            const cup::Cup* cup() const;

            void add_cup_listener(cup::Cup_listener* cup_listener);
            void remove_cup_listener(cup::Cup_listener* cup_listener);

            const game::Stage_loader* async_load_stage(const cup::Stage_data& stage_data, std::function<void(const action::Stage*)> completion_callback);
            const action::Stage* stage() const;

            void end_action();
            void clean_stage();

        private:
            std::unique_ptr<impl::Server> impl_;
        };
    }
}

#endif