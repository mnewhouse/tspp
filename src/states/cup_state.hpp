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
#include "user_interface/elements/elements.hpp"


namespace ts
{
    namespace states
    {
        class Car_selection_dialog;

        class Cup_state
            : public gui::State, public game::Cup_listener
        {
        public:
            Cup_state(game::Cup_type cup_type, state_machine_type* state_machine, gui::Context* context,
                      resources::Resource_store* resource_store);

            virtual void on_state_change(game::Cup_state old_state, game::Cup_state new_state) override;
            virtual void on_restart() override;

            virtual void update(std::size_t frame_duration) override;
            void launch_action();

            virtual void on_activate();

            void load_car_selection_dialog();
            void show_car_selection_dialog();

            void confirm_car_selection();

        private:
            utf8_string cup_state_to_string(game::Cup_state cup_state) const;
            void add_selected_local_players();

            gui::Document_handle create_cup_document();
            gui::Document_handle create_progress_window();

            utf8_string loading_phase_to_info_string(game::Loading_phase phase);
            void apply_car_selection();

            void return_to_main_menu();

            game::Action_loader action_loader_;
            game::Cup cup_;
            
            gui::Text_element* header_text_;

            gui::Text_element* progress_info_text_;
            gui::Progress_bar* progress_bar_;
            sf::Clock complete_timer_;

            std::atomic<bool> car_selection_ready_ = false;

            game::Loading_phase loading_phase_ = game::Loading_phase::Initializing;

            std::unique_ptr<Car_selection_dialog> car_selection_dialog_;

            gui::Document_handle cup_document_;
            gui::Document_handle progress_document_;
        };

        class Car_selection_dialog
        {
        public:
            Car_selection_dialog(Cup_state* cup_state);

            void load(std::vector<game::Cup::Player_handle> selected_players, std::vector<resources::Car_handle> possible_cars, resources::Track_handle track_handle);

            void show();
            void hide();

            const std::vector<std::pair<game::Cup::Player_handle, resources::Car_handle>>& selected_cars() const;

        private:
            void load_dialog();
            void load_car_textures();
            void confirm();

            gui::Document_handle create_car_selection_dialog();

            Cup_state* cup_state_;

            std::vector<game::Cup::Player_handle> selected_players_;
            std::vector<resources::Car_handle> possible_cars_;
            resources::Track_handle track_handle_;

            std::vector<std::shared_ptr<graphics::Texture>> player_car_textures_;
            std::vector<Int_rect> texture_mapping_;

            std::vector<std::pair<game::Cup::Player_handle, resources::Car_handle>> selected_cars_;

            std::future<void> loading_future_;

            gui::Vertical_list<gui::Element>* selection_list_;

            gui::Document_handle car_selection_document_;
        };
    }
}

#endif
