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

#ifndef RESOURCES_SETTINGS_HPP
#define RESOURCES_SETTINGS_HPP

#include "track_handle.hpp"
#include "car_handle.hpp"
#include "player_store.hpp"

#include "controls/key_mapping.hpp"

namespace ts
{
    namespace resources
    {
        class Car_store;
        class Track_store;

        struct Video_settings
        {
            bool full_screen = true;
            bool vertical_sync = false;
            Vector2u screen_resolution;
            Vector2u current_screen_resolution;
            double zoom_level = 1.0;
        };

        struct Input_settings
        {
            controls::Key_mapping key_mapping;
        };

        struct Audio_settings
        {
            double effects_volume = 0.8;
            double engine_volume = 0.8;
            double remote_engine_volume = 0.5;

            std::size_t engine_channels = 8;
            std::size_t effects_channels = 8;
        };

        class Game_settings
        {
        };

        struct Track_settings
        {
            Track_settings(const Track_store* track_store_)
                : track_store(track_store_)
            {}

            const Track_store* track_store;
            std::vector<Track_handle> selected_tracks;
        };

        enum class Car_mode
        {
            Fixed,
            Free,
            Random
        };

        struct Script_settings
        {
            std::set<utf8_string> loaded_scripts;
        };

        struct Car_settings
        {
            Car_settings(const Car_store* car_store)
                : car_store_(car_store)
            {}

            void select_car(Car_handle car);
            void deselect_car(Car_handle car);

            const std::vector<Car_handle>& selected_cars() const;

            void set_car_mode(Car_mode car_mode);
            Car_mode car_mode() const;

            const Car_store* car_store() const;

            bool is_car_selected(Car_handle car) const;

        private:
            const Car_store* car_store_;
            std::vector<Car_handle> selected_cars_;
            Car_mode car_mode_ = Car_mode::Free;
        };

        struct Player_settings
        {
            Player_settings();

            std::array<Player_store::unique_id, 4> selected_players;
        };

        struct Settings
        {
            Settings(utf8_string file_name, const Car_store* car_store, const Track_store* track_store);
            ~Settings();

            Video_settings video_settings;
            Input_settings input_settings;
            Audio_settings audio_settings;
            Game_settings game_settings;
            Track_settings track_settings;
            Car_settings car_settings;
            Player_settings player_settings;
            Script_settings script_settings;

        private:
            utf8_string file_name_;
        };        
    }
}

#endif