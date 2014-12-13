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

#ifndef LOCAL_INTERACTIONS_HPP
#define LOCAL_INTERACTIONS_HPP

#include "client/client_messages.hpp"

#include "cup/chatbox.hpp"

namespace ts
{
    namespace client
    {
        class Chatbox_interface
            : public Message_listener
        {
        public:
            Chatbox_interface(Message_center* message_center);

            virtual void handle_message(const Server_message& message) override;

            const cup::Chatbox* chatbox() const;
            cup::Chatbox* chatbox();

            core::Listener_host<cup::Chatbox_listener>* chatbox_listener_host();

        private:
            void handle_chatbox_output(const Message& message);

            cup::Chatbox chatbox_;
        };
    }
}

#endif