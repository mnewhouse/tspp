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

#ifndef STATES_MAIN_MENU_HPP
#define STATES_MAIN_MENU_HPP

#include "user_interface/gui_state.hpp"
#include "user_interface/document.hpp"

namespace ts
{
    namespace states
    {
        class Player_setup_menu;
        class Player_selection_menu;
        class Settings_menu;

        class Cup_setup_menu;

        class Main_menu
            : public gui::State
        {
        public:
            Main_menu(state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store);            

            void show();

            virtual void on_activate() override;

        private:
            void show_player_setup();
            void show_local_cup_setup();
            void show_settings_menu();
            void quit();

            gui::Document_handle create_main_document();
            
            gui::Document_handle background_document_;
            gui::Document_handle main_document_;

            std::unique_ptr<Player_setup_menu> player_setup_;
            std::unique_ptr<Cup_setup_menu> cup_setup_;
            std::unique_ptr<Settings_menu> settings_menu_;
        };

    }
}

#endif