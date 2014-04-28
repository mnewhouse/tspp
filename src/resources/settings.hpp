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

#include "core/vector2.hpp"

#include "controls/key_mapping.hpp"

#include <vector>

namespace ts
{
    namespace resources
    {
        struct Video_settings
        {
            bool full_screen = true;
            Vector2u screen_resolution;
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
            double skid_volume = 0.8;
            double remote_engine_volume = 0.5;
        };

        class Game_settings
        {
        };

        struct Track_settings
        {
            std::vector<std::string> selected_tracks;
            bool allow_duplicates = true;
            std::uint32_t random_count = 1;
        };

        struct Settings
        {
            Video_settings video_settings;
            Input_settings input_settings;
            Audio_settings audio_settings;
            Game_settings game_settings;
            Track_settings track_settings;
        };        


        Settings load_settings(const std::string& file_name);
        Settings load_settings(std::istream& stream);

        void save_settings(const std::string& file_name, const Settings& settings);
    }
}

#endif