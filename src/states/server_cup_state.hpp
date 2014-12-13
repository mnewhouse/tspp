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

#include "cup_state_base.hpp"

#include "server/server.hpp"
#include "client/local_client.hpp"

namespace ts
{
    namespace states
    {
        class Server_action_state;

        namespace impl
        {
            struct Server_cup_state_members
            {
                Server_cup_state_members(resources::Resource_store* resource_store);

                server::Server server_;
                client::Local_client local_client_;
            };
        }

        class Server_cup_state
            : private impl::Server_cup_state_members, public Cup_state_base
        {
        public:
            Server_cup_state(state_machine_type* state_machine, gui::Context* context,
                             resources::Resource_store* resource_store);

            virtual ~Server_cup_state();

            virtual void update(std::size_t frame_duration) override;

            void listen(std::uint16_t port);

        private:
            virtual void on_initialize(const cup::Stage_data& stage_data) override;
            virtual void on_activate() override;

            virtual std::unique_ptr<Action_state_base> make_action_state() override;
        };
    }
}

#endif