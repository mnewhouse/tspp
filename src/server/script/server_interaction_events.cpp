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
#include "server_interaction_events.hpp"
#include "server_script_events.hpp"
#include "server_script_engine.hpp"

#include "server/client_map.hpp"
#include "server/generic_client.hpp"

#include "script/userdata_forwarder.hpp"
#include "script/script_engine.hpp"
#include "script/script_module.hpp"

ts::server::Interaction_event_listener::Interaction_event_listener(Interaction_interface* interaction_interface,
                                                                   Script_engine* script_engine, const Client_map* client_map)
: Interaction_listener(interaction_interface),
  interaction_interface_(interaction_interface),
  script_engine_(script_engine)
{
}

void ts::server::Interaction_event_listener::on_client_connect(const Generic_client& client)
{
    script_engine_->trigger_event_generic(script_events::onClientConnect, nullptr, script::forward_as_userdata(client));
}

void ts::server::Interaction_event_listener::on_client_disconnect(const Generic_client& client)
{
    script_engine_->trigger_event_generic(script_events::onClientDisconnect, nullptr, script::forward_as_userdata(client));
}