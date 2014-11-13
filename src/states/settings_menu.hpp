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

namespace ts
{
    namespace resources
    {
        struct Settings;
    }

    namespace states
    {
        class Main_menu;
        class Control_settings_menu;
        class Video_settings_menu;
        class Network_settings_menu;

        class Settings_menu
        {
        public:
            Settings_menu(Main_menu* main_menu);
            ~Settings_menu();

            void show();
            void hide();

        private:
            void show_key_setup();
            void show_video_settings();
            void show_network_settings();

            void hide_all_submenus();
            void return_to_main_menu();

            gui::Document_handle create_navigation(gui::Context* context);

            Main_menu* main_menu_;
            resources::Settings* settings_;

            gui::Document_handle settings_navigation_;
            std::unique_ptr<Control_settings_menu> control_settings_;
            std::unique_ptr<Video_settings_menu> video_settings_;
            std::unique_ptr<Network_settings_menu> network_settings_;
        };
    }
}

#endif