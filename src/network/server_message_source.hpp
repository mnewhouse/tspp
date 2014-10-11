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

#ifndef SERVER_MESSAGE_SOURCE_HPP
#define SERVER_MESSAGE_SOURCE_HPP

namespace ts
{
    namespace network
    {
        class Server;
        struct Client_message;

        class Server_message_source
        {
        public:
            Server_message_source(Server* server);

            bool operator()(Client_message& client_message) const;

        private:
            Server* server_;
        };
    }
}

#endif