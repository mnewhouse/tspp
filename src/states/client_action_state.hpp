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

#ifndef CLIENT_ACTION_STATE_HPP
#define CLIENT_ACTION_STATE_HPP

#include "action_state_base.hpp"

namespace ts
{
    namespace client
    {
        class Client;
    }

    namespace states
    {
        class Client_action_state
            : public Action_state_base
        {
        public:
            Client_action_state(game::Loaded_scene loaded_scene, client::Client* client, 
                                state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store);

            virtual ~Client_action_state();

            virtual void update(std::size_t frame_duration) override;
            virtual void on_activate() override;

        private:            
            client::Client* client_;
        };
    }
}

#endif