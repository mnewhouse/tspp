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

#ifndef STATES_PLAYER_SETUP_HPP
#define STATES_PLAYER_SETUP_HPP

#include "main_menu.hpp"

#include "user_interface/elements/elements.hpp"
#include "user_interface/elements/player_color_background.hpp"

namespace ts
{
    namespace states
    {
        class Player_selection_menu;
        class Color_selection_menu;

        class Player_setup_menu
        {
        public:
            Player_setup_menu(Main_menu* main_menu);

            void hide();
            void show();
            void update_selection();

            gui::Context* context() const;
            resources::Resource_store* resource_store() const;
            
            void show_color_selection_dialog(resources::Player_handle player_handle);
            void return_from_color_selection(resources::Player_handle player, resources::Player_color new_color);

        private:
            void goto_player_selection(int slot);
            void return_to_main_menu();

            gui::Document_handle create_player_setup_document();

            struct Slot
            {
                int slot_id;
                gui::Text_element* name_text = nullptr;
                std::shared_ptr<gui::Player_color_background> color_background;
                gui::Element* color_element;
            };

            Slot slots_[4];

            Main_menu* main_menu_;
            gui::Document_handle player_setup_document_;

            std::unique_ptr<Player_selection_menu> player_selection_menu_;
            std::unique_ptr<Color_selection_menu> color_selection_menu_;

            bool return_to_player_selection_ = false;
        };


        class Player_selection_menu
        {
        public:
            Player_selection_menu(Player_setup_menu* player_setup_menu);

            void show();
            void show(int slot);
            void hide();

            void deactivate();
            void activate();

            void update_color_preview(resources::Player_handle player_handle, resources::Player_color new_color);

        private:
            void return_to_player_setup();
            void deselect_all();
            void select_player(resources::Player_handle player);

            gui::Document_handle create_player_selection_document();
            gui::Document_handle create_player_deletion_dialog();
            gui::Document_handle create_player_creation_dialog();

            void register_selection_handler(gui::Element* player_name_elem, resources::Player_handle player_handle);

            resources::Player_handle create_player(const utf8_string& name);
            void delete_player();

            void remove_none_row();
            void create_player_list_row(resources::Player_handle player_handle);
            void scroll_down();

            void hide_player_deletion_dialog();
            void show_player_deletion_dialog();

            void show_player_creation_dialog();
            void hide_player_creation_dialog();

            using player_list_type = gui::Vertical_list<gui::Element>;

            int current_slot_ = 0;
            player_list_type* player_list_;
            Player_setup_menu* player_setup_menu_;

            gui::Scroll_pane* scroll_pane_;

            gui::Edit_box* player_editbox_ = nullptr;
            gui::Element* selected_player_row_ = nullptr;
            resources::Player_handle selected_player_;

            gui::Text_style list_text_style_;
            gui::Text_style list_text_hover_style_;

            gui::Text_element* player_deletion_text_;

            std::multimap<resources::Player_handle, std::shared_ptr<gui::Player_color_background>> color_preview_map_;

            gui::Document_handle player_selection_document_;
            gui::Document_handle player_deletion_dialog_;
            gui::Document_handle player_creation_dialog_;
        };

        class Color_selection_menu
        {
        public:
            Color_selection_menu(Player_setup_menu* player_setup_menu);

            void show(resources::Player_handle player_handle);
            void hide();

        private:
            void set_color(resources::Player_color color);
            void return_to_previous();

            gui::Document_handle create_color_dialog();

            void update_color_pickers(sf::Color primary_color, sf::Color secondary_color);
            void update_color_preview();

            void select_base_component();
            void select_secondary_component();
            void select_tertiary_component();

            void update_color_from_sliders();
            void update_color_from_input();

            void set_base_component_type(resources::Color_base::Type type);
            void set_secondary_component_type(resources::Color_extra::Type type);
            void set_tertiary_component_type(resources::Color_extra::Type type);

            void update_component_types();

            Player_setup_menu* player_setup_menu_;

            resources::Player_handle player_handle_;
            resources::Player_color color_;

            struct Color_picker
            {
                std::shared_ptr<gui::Plain_background> preview_background;

                gui::Slider_bar* red_slider;
                gui::Slider_bar* green_slider;
                gui::Slider_bar* blue_slider;

                gui::Integer_input* red_input;
                gui::Integer_input* green_input;
                gui::Integer_input* blue_input;
            };

            gui::Text_element* player_name_text_;
            gui::Text_element* base_component_text_;
            gui::Text_element* secondary_component_text_;
            gui::Text_element* tertiary_component_text_;

            gui::Option_set<resources::Color_base::Type>* base_component_option_set_;
            gui::Option_set<resources::Color_extra::Type>* secondary_component_option_set_;
            gui::Option_set<resources::Color_extra::Type>* tertiary_component_option_set_;

            std::shared_ptr<gui::Player_color_background> player_color_background_;

            Color_picker primary_color_picker_;
            Color_picker secondary_color_picker_;

            int selected_component_;
            bool update_in_progress_ = false;

            gui::Document_handle color_dialog_;
        };
    }
}

#endif