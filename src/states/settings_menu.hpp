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

#ifndef STATES_SETTINGS_MENU_HPP
#define STATES_SETTINGS_MENU_HPP

#include "user_interface/document_handle.hpp"

#include "resources/settings.hpp"
#include "resources/settings_copy.hpp"

#include "user_interface/elements/elements.hpp"

namespace ts
{
    namespace states
    {
        class Main_menu;
        class Control_settings_menu;
        class Video_settings_menu;

        class Settings_menu
        {
        public:
            Settings_menu(Main_menu* main_menu);

            void show();
            void hide();

        private:
            void show_key_setup();
            void show_video_settings();

            void hide_all_submenus();
            void return_to_main_menu();

            gui::Document_handle create_navigation(gui::Context* context);

            Main_menu* main_menu_;
            resources::Settings* settings_;

            gui::Document_handle settings_navigation_;
            std::unique_ptr<Control_settings_menu> control_settings_;
            std::unique_ptr<Video_settings_menu> video_settings_;
        };
        
        class Control_settings_menu
        {
        public:
            Control_settings_menu(gui::Context* context, resources::Input_settings* input_settings);

            void show();
            void hide();

        private:
            void create_document(gui::Context* context);
            void set_current_slot(controls::Slot new_slot);

            gui::Context* context_;
            void assign_key_to_pending_control(sf::Keyboard::Key key);
            void cancel_control_assignment();
            void start_control_assignment(gui::Text_element* key_text, controls::Control control);

            void load_key_binds(const resources::Input_settings& input_settings, controls::Slot slot);

            resources::Settings_copy<resources::Input_settings> input_settings_;

            struct Key_bind_text
            {
                gui::Text_element* accelerate = nullptr;
                gui::Text_element* brake = nullptr;
                gui::Text_element* left = nullptr;
                gui::Text_element* right = nullptr;
                gui::Text_element* fire = nullptr;
            };

            gui::Integer_input* slot_selection_ = nullptr;
            Key_bind_text key_bind_text_;
            controls::Control input_control_ = controls::Control::None;
            gui::Text_element* input_key_text_ = nullptr;
            controls::Slot current_slot_ = controls::invalid_slot;
            

            gui::Document_handle document_;
        };

        class Video_settings_menu
        {
        public:
            Video_settings_menu(gui::Context* context, resources::Video_settings* video_settings);

            void show();
            void hide();

        private:
            void create_document(gui::Context* context);

            gui::Context* context_;
            resources::Settings_copy<resources::Video_settings> video_settings_;
            gui::Document_handle document_;            

            using resolution_option_set_type = gui::Option_set<std::pair<std::int32_t, std::int32_t>, gui::option_set::no_wrap>;
            resolution_option_set_type* resolution_option_set_ = nullptr;

            gui::Option_set<bool>* full_screen_option_set_ = nullptr;
            gui::Option_set<bool>* vsync_option_set_ = nullptr;
        };
    }
}

#endif