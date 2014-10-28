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

#ifndef STATES_CUP_SETUP_HPP
#define STATES_CUP_SETUP_HPP

#include "main_menu.hpp"

#include "user_interface/document_handle.hpp"
#include "user_interface/elements/elements.hpp"

#include "resources/car_identifier.hpp"
#include "resources/settings/car_settings.hpp"
#include "resources/track_store.hpp"

namespace ts
{
    namespace states
    {
        class Track_setup_menu;
        class Car_setup_menu;
        class Network_menu;

        class Cup_setup_menu
        {
        public:
            Cup_setup_menu(Main_menu* main_menu);

            void show();
            void hide();
            gui::Element* create_containment_window(gui::Document* parent, utf8_string title);

            resources::Resource_store* resource_store();
            gui::Context* context();

            void check_startability();

            void disable_networking();
            void enable_networking(Network_menu* network_menu);

        private:
            void start_cup();

            void return_to_main_menu();
            void show_track_setup();
            void show_car_setup();

            void create_navigation_document(gui::Context* context);

            std::unique_ptr<Track_setup_menu> track_setup_menu_;
            std::unique_ptr<Car_setup_menu> car_setup_menu_;

            Network_menu* network_menu_ = nullptr;
            Main_menu* main_menu_ = nullptr;
            gui::Text_element* start_button_ = nullptr;
            gui::Text_element* network_button_ = nullptr;

            gui::Document_handle navigation_document_;            
        };

        class Track_setup_menu
        {
        public:
            Track_setup_menu(Cup_setup_menu* cup_setup_menu);

            void show();
            void hide();

        private:
            void populate_selected_tracks_list();
            void append_selected_track(resources::Track_handle track);
            utf8_string make_selected_track_string(resources::Track_handle track, std::size_t selection_index);

            void open_directory(resources::Track_directory directory);
            void add_track(resources::Track_handle track);

            void clear_selection();
            void remove_selection();

            void move_selection_up();
            void move_selection_down();

            void select_track(std::size_t index = 0);
            void select_random_tracks();

            void show_random_selection_dialog();
            void hide_random_selection_dialog();

            gui::Document_handle create_track_setup_document(Cup_setup_menu* cup_setup_menu);
            gui::Document_handle create_random_selection_dialog(Cup_setup_menu* cup_setup_menu);

            Cup_setup_menu* cup_setup_menu_;

            using track_grid_type = gui::Grid_list<gui::Element>;
            track_grid_type* track_grid_list_;

            using selection_list_type = gui::Vertical_list<gui::Text_element>;
            selection_list_type* track_selection_list_;

            gui::Text_element* selected_tracks_text_;
            gui::Text_element* remove_selection_button_;
            gui::Text_element* move_up_button_;
            gui::Text_element* move_down_button_;

            gui::Text_style selected_track_style_;
            gui::Text_style selected_track_hover_style_;

            gui::Scroll_pane* selection_scroll_pane_;

            std::size_t selection_index_ = 0;

            resources::Track_directory current_directory_;

            gui::Text_element* allow_duplicates_text_;
            gui::Integer_input* track_number_input_;
            bool allow_duplicates_ = false;

            gui::Document_handle track_setup_document_;
            gui::Document_handle random_selection_dialog_;
        };

        class Car_setup_menu
        {
        public:
            Car_setup_menu(Cup_setup_menu* cup_setup_menu);

            void show();
            void hide();

        private:
            void toggle_car_selection(const resources::Car_identifier& car_identifier);
            void traverse_car_mode();
            void update_selection_rows();
            utf8_string car_mode_to_string(resources::Car_mode car_mode) const;

            gui::Document_handle create_car_setup_document(Cup_setup_menu* cup_setup_menu);

            resources::Car_settings* car_settings_;

            Cup_setup_menu* cup_setup_menu_;

            struct Car_row
            {
                resources::Car_identifier car_identifier;
                gui::Element* row_elem;
            };

            std::vector<Car_row> car_rows_;
            
            gui::Text_element* car_mode_setting_string_;

            gui::Document_handle car_setup_document_;
        };
    }
}

#endif