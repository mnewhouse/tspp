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

#ifndef CLIENT_INTERACTIONS_HPP
#define CLIENT_INTERACTIONS_HPP

#include "client_messages.hpp"
#include "registration.hpp"

namespace ts
{
    namespace cup
    {
        class Cup;
    }

    namespace resources
    {
        struct Resource_store;
    }

    namespace client
    {
        class Interaction_interface
        {
        public:
            Interaction_interface(Message_center* message_center, cup::Cup* cup, resources::Resource_store* resource_store);
            ~Interaction_interface();

            void send_registration_request();
            void poll();

            Registration_status registration_status() const;
            const utf8_string& registration_error() const;

            bool is_downloading() const;
            std::pair<std::size_t, std::size_t> download_progress() const;

        private:
            class Impl;
            std::unique_ptr<Impl> impl_;
        };
    }
}

#endif