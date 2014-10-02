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

#ifndef LOCAL_CUP_STATE_HPP
#define LOCAL_CUP_STATE_HPP

#include "cup_state_base.hpp"

#include "cup/cup.hpp"
#include "cup/cup_listener.hpp"
#include "cup/local_cup_interface.hpp"

namespace ts
{
    namespace states
    {
        namespace impl
        {
            struct Local_cup_state_members
            {
                Local_cup_state_members(resources::Resource_store* resource_store);

                cup::Cup cup_;
                cup::Local_cup_interface local_cup_interface_;
            };
        }

        class Local_cup_state
            : private impl::Local_cup_state_members, public Cup_state_base
        {
        public:
            Local_cup_state(state_machine_type* state_machine, gui::Context* context,
                            resources::Resource_store* resource_store);

            virtual ~Local_cup_state();

            virtual void on_state_change(cup::Cup_state old_state, cup::Cup_state new_state) override;
            virtual void update(std::size_t frame_duration) override;

        private:
            virtual std::unique_ptr<Action_state_base> create_action_state(game::Loaded_scene loaded_scene) override;

        };
    }
}

#endif