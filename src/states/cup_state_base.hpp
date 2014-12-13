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

#include "user_interface/gui_state.hpp"

#include "cup/cup_listener.hpp"

namespace ts
{
    namespace client
    {
        class Client_interface;
    }

    namespace game
    {
        struct Loaded_scene;
    }

    namespace resources
    {
        class Loading_interface;
    }


    namespace states
    {
        class Action_state_base;
        class Cup_GUI;

        class Cup_state_base
            : public gui::State, protected cup::Scoped_cup_listener
        {
        public:
            Cup_state_base(const client::Client_interface* client_interface,
                state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store);

            virtual ~Cup_state_base();

            void show_gui();            

            virtual void update(std::size_t frame_duration) override;
            virtual void on_activate() override;

            virtual void on_state_change(cup::Cup_state old_state, cup::Cup_state new_state) override;

        protected:
            void show_stage_loading(const resources::Loading_interface* loading_interface);
            void ready_for_action();
            
            void launch_action(std::unique_ptr<Action_state_base> action_state);
            virtual std::unique_ptr<Action_state_base> make_action_state() = 0;

        private:
            void return_to_main_menu();

            const client::Client_interface* client_interface_;
            const resources::Loading_interface* loading_interface_ = nullptr;

            std::unique_ptr<Cup_GUI> cup_gui_;            
        };
    }
}

#endif
