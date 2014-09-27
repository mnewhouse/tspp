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

#ifndef STATES_CUP_STATE_HPP
#define STATES_CUP_STATE_HPP

#include "game/cup.hpp"
#include "game/cup_listener.hpp"
#include "game/action_loader.hpp"

#include "user_interface/gui_state.hpp"

#include "gui_definitions/cup_gui.hpp"


namespace ts
{
    namespace states
    {
        class Action_state_base;

        class Cup_state_base
                : public gui::State, public game::Cup_listener
        {
        public:
            Cup_state_base(game::Cup_type cup_type, state_machine_type* state_machine, gui::Context* context,
                    resources::Resource_store* resource_store);

            virtual ~Cup_state_base();            

            // Cup listener overrides
            virtual void on_state_change(game::Cup_state old_state, game::Cup_state new_state) override;
            virtual void on_restart() override;
            virtual void on_end() override;

            // State overrides
            virtual void on_activate() override;
            virtual void update(std::size_t frame_duration) override;

        protected:
            game::Cup* cup();
            void add_selected_local_players();

        private:
            virtual std::unique_ptr<Action_state_base> create_action_state(game::Loaded_scene) = 0;

            void launch_action();
            void return_to_main_menu();

            game::Cup cup_;
            game::Action_loader action_loader_;

            game::Loading_phase loading_phase_ = game::Loading_phase::None;
            std::chrono::high_resolution_clock::time_point completion_time_;

            Cup_GUI cup_gui_;
        };

        template <game::Cup_type CupType>
        class Cup_state;
    }
}

#endif
