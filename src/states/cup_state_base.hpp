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

#ifndef STATES_CUP_STATE_BASE_HPP
#define STATES_CUP_STATE_BASE_HPP

#include "cup_gui.hpp"

#include "cup/cup_listener.hpp"

#include "user_interface/gui_state.hpp"

#include "game/loaded_scene.hpp"



namespace ts
{
    namespace cup
    {
        class Cup_interface;
    }

    namespace game
    {
        class Loading_sequence;
    }

    namespace states
    {
        class Action_state_base;

        class Cup_state_base
                : public cup::Cup_listener, public cup::Chatbox_listener,
                  public gui::State
        {
        public:
            Cup_state_base(cup::Cup_interface* cup_interface, state_machine_type* state_machine, gui::Context* context,
                           resources::Resource_store* resource_store);

            virtual ~Cup_state_base();

            // Cup listener overrides
            virtual void on_state_change(cup::Cup_state old_state, cup::Cup_state new_state) override;
            virtual void on_restart() override;
            virtual void on_end() override;

            void show_gui();

            void begin_loading_sequence(const cup::Stage_data& stage_data);

            virtual void update(std::size_t frame_duration) override;
            virtual void on_activate() override;

            virtual void on_chat_message(const cup::Composite_message& message) override;

        protected:
            void launch_action(std::unique_ptr<Action_state_base> action_state);
            game::Loaded_scene transfer_loaded_scene();           

        private:
            void return_to_main_menu();

            cup::Cup_interface* cup_interface_;

            std::chrono::high_resolution_clock::time_point completion_time_;

            Cup_GUI cup_gui_;

            std::unique_ptr<game::Loading_sequence> loading_sequence_;
        };
    }
}

#endif
