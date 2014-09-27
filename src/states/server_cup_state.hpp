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

#ifndef STATES_SERVER_CUP_STATE_HPP
#define STATES_SERVER_CUP_STATE_HPP

#include "cup_state.hpp"

#include "network/server.hpp"

namespace ts
{
    namespace states
    {
        class Server_cup_state
            : public Cup_state_base
        {
        public:
            Server_cup_state(state_machine_type* state_machine, gui::Context* context,
                             resources::Resource_store* resource_store);

            virtual ~Server_cup_state();

        private:
            virtual std::unique_ptr<Action_state_base> create_action_state(game::Loaded_scene loaded_scene) override;

            // Cup listener overrides
            virtual void on_state_change(game::Cup_state old_state, game::Cup_state new_state) override;

            network::Server server_;
        };
    }
}

#endif