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

#ifndef SERVER_INTERACTIONS_HPP
#define SERVER_INTERACTIONS_HPP

#include "server_messages.hpp"

namespace ts
{
    namespace cup
    {
        class Cup_controller;
    }

    namespace server
    {
        class Client_map;
        class Interaction_listener;
        class Stage_interface;
        class Command_center;

        class Interaction_interface
        {
        public:
            Interaction_interface(Message_center* message_center, Client_map* client_map, cup::Cup_controller* cup_controller,
                                  const Stage_interface* stage_interface, const Command_center* command_center);

            ~Interaction_interface();

            void add_interaction_listener(Interaction_listener* listener);
            void remove_interaction_listener(Interaction_listener* listener);

        private:
            class Impl;

            std::unique_ptr<Impl> impl_;
        };
    }
}

#endif