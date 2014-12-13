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

#ifndef SERVER_SCRIPT_EVENTS_HPP
#define SERVER_SCRIPT_EVENTS_HPP

namespace ts
{
    namespace server
    {
        namespace script_events
        {
            // void onClientConnect(Client client)
            static const char onClientConnect[] = "onClientConnect";

            // void onClientDisconnect(Client client)
            static const char onClientDisconnect[] = "onClientDisconnect";
            
            // void onPlayerJoin(Player player)
            static const char onPlayerJoin[] = "onPlayerJoin";

            // void onPlayerLeave(Player player)
            static const char onPlayerLeave[] = "onPlayerLeave";

            // void onStagePreinitialize(StageAssmbler stage_assembler);
            static const char onStagePreinitialize[] = "onStagePreInitialize";

            // void onGameStart();
            static const char onGameStart[] = "onGameStart";

            // void onGameEnd();
            static const char onGameEnd[] = "onGameEnd";
        }
    }
}

#endif