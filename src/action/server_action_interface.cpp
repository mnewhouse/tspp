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

#include "stdinc.hpp"
#include "server_action_interface.hpp"

#include "network/server.hpp"

ts::action::Server_action_interface::Server_action_interface(Stage* stage, const cup::Client_player_mapping* client_player_mapping,
                                                             listener_set_type* listener_set)
: Message_listener(listener_set),
  stage_(stage),
  client_player_mapping_(client_player_mapping)
{
}

ts::action::Server_action_interface::~Server_action_interface()
{
}

void ts::action::Server_action_interface::handle_message(const network::Client_message& client_message)
{
    const auto& message = client_message.message;
}
