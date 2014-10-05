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

#ifndef STATES_CUP_GUI_HPP
#define STATES_CUP_GUI_HPP

#include "cup/cup_interface.hpp"

#include "resources/track_handle.hpp"

#include "user_interface/gui_context.hpp"
#include "user_interface/elements/elements.hpp"

namespace ts
{
    namespace resources
    {
        struct Resource_store;
    }

    namespace states
    {
        class Car_selection_dialog;
        class Loading_progress_dialog;

        class Cup_GUI
        {
        public:
            Cup_GUI(cup::Cup_interface* cup_interface, gui::Context* context, const resources::Resource_store* resource_store);

            void show();
            void hide();

            void update(std::size_t frame_duration);
            void set_cup_state_text(utf8_string text);

            void show_car_selection_dialog();      
            Loading_progress_dialog* progress_dialog();

            void output_chat_message(const cup::Composite_message& message);
            

        private:
            void show_menu_background();

            utf8_string cup_state_to_string(cup::Cup_state cup_state) const;
            void add_selected_local_players();

            void create_cup_document(gui::Context* context);

            void apply_car_selection();
            void return_to_main_menu();

            friend class Car_selection_dialog;
            void confirm_car_selection();
            void car_selection_dialog_ready();
            
            cup::Cup_interface* cup_interface_;
            const cup::Cup* cup_;

            gui::Context* context_;
            gui::Text_element* header_text_;

            using chatbox_type = gui::Vertical_list<gui::Text_element>;
            gui::Text_style chatbox_text_style_;
            chatbox_type* chatbox_;

            std::atomic<bool> car_selection_ready_;

            gui::Document_handle cup_document_;

            std::unique_ptr<Car_selection_dialog> car_selection_dialog_;
            std::unique_ptr<Loading_progress_dialog> progress_dialog_;            
        };

        class Loading_progress_dialog
        {
        public:
            Loading_progress_dialog(gui::Context* context);

            void show();
            void hide();

            bool visible() const;

            void set_loading_state(utf8_string state);
            void set_progress(double progress);
            void set_max_progress(double max_progress);

        private:
            void create_progress_document(gui::Context* context);
            
            gui::Text_element* progress_info_text_ = nullptr;
            gui::Progress_bar* progress_bar_ = nullptr;

            gui::Document_handle progress_document_;
        };
        
        class Car_selection_dialog
        {
        public:
            Car_selection_dialog(gui::Context* context, Cup_GUI* cup_gui);

            void load(std::vector<cup::Player_handle> selected_players, std::vector<resources::Car_handle> possible_cars, resources::Track_handle track_handle);

            void show();
            void hide();

            const std::vector<cup::Car_selection>& car_selection() const;

        private:
            void load_dialog();
            void load_car_textures();
            void confirm();

            void create_car_selection_dialog(gui::Context* context);

            Cup_GUI* cup_gui_;
            gui::Context* context_;

            std::vector<cup::Player_handle> selected_players_;
            std::vector<resources::Car_handle> possible_cars_;
            resources::Track_handle track_handle_;

            std::vector<std::shared_ptr<graphics::Texture>> player_car_textures_;
            std::vector<Int_rect> texture_mapping_;

            std::vector<cup::Car_selection> car_selection_;

            std::future<void> loading_future_;

            gui::Vertical_list<gui::Element>* selection_list_;

            gui::Document_handle car_selection_document_;
        };
    }
}

#endif