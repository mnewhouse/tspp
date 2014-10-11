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

#ifndef CUP_SERVER_HPP
#define CUP_SERVER_HPP

#include "messages/message.hpp"

namespace ts
{
    namespace cup
    {
        using Client_id = std::uint32_t;

        struct Client_message
        {
            Client_id client_id;
            messages::Message message;
        };

        class Cup_server
        {
        public:
            Cup_server(std::uint16_t listen_port);
            ~Cup_server();

            bool get_message(Client_message& client_message) const;
            bool send_message(const Client_message& client_message) const;
            bool send_message_to_all(const messages::Message& message) const;

            void disconnect_client(Client_id client_id);

        private:
            struct Impl;
            std::unique_ptr<Impl> impl_;
        };
    }
}

#endif