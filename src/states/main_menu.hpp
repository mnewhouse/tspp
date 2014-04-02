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

#ifndef STATES_MAIN_MENU_HPP
#define STATES_MAIN_MENU_HPP

#include "user_interface/gui_state.hpp"
#include "user_interface/gui_scene.hpp"

#include "resources/resource_store.hpp"

namespace ts
{

    namespace states
    {
        class Settings_scene;
        class Main_menu_scene;
        class Players_scene;

        class Main_menu
            : public gui::State
        {
        public:
            Main_menu(state_machine_type* state_machine, gui::Context* gui_context,
                      resources::Resource_store* resource_store);

            virtual void render(graphics::Render_target& render_target) override;
            virtual void update(std::size_t frame_duration) override;
            virtual void handle_event(const sf::Event& event) override;

            void quit();
            void return_to_main_menu();
            void show_settings_menu();
            void show_players_menu();

        private:
            std::unique_ptr<Main_menu_scene> main_menu_scene_;
            std::unique_ptr<Settings_scene> settings_scene_;
            std::unique_ptr<Players_scene> players_scene_;
        };

    }

}

#endif