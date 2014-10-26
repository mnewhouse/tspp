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

#ifndef CLIENT_INTERFACE_HPP
#define CLIENT_INTERFACE_HPP

#include "cup/cup_metadata.hpp"

#include "client_messages.hpp"

namespace ts
{
    namespace cup
    {
        class Cup;
    }

    namespace client
    {
        using cup::Car_selection;

        class Client_interface
        {
        public:
            Client_interface(Message_center* message_center, const cup::Cup* cup);
            ~Client_interface();

            void select_cars(const std::vector<Car_selection>& car_selection) const;
            void signal_ready() const;
            void write_message(const utf8_string& message) const;
            void request_advance() const;
            void quit() const;

            const cup::Cup* cup() const;
            Message_center* message_center() const;

        private:
            Message_center* message_center_;
            const cup::Cup* cup_;

            mutable Server_message message_buffer_;
        };
    }
}

#endif