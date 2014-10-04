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

#include "settings/track_settings.hpp"
#include "settings/car_settings.hpp"
#include "settings/video_settings.hpp"
#include "settings/script_settings.hpp"
#include "settings/input_settings.hpp"
#include "settings/player_settings.hpp"
#include "settings/network_settings.hpp"
#include "settings/audio_settings.hpp"

namespace ts
{
    namespace resources
    {
        class Car_store;

        struct Cup_settings
        {
            std::uint16_t max_players = 20;
            std::uint16_t max_spectators = 10;
        };

        struct Settings
        {
            Settings(utf8_string file_name);
            ~Settings();

            Video_settings video_settings;
            Input_settings input_settings;
            Audio_settings audio_settings;
            Track_settings track_settings;
            Car_settings car_settings;
            Player_settings player_settings;
            Script_settings script_settings;
            Network_settings network_settings;
            Cup_settings cup_settings;

        private:
            utf8_string file_name_;
        };        
    }
}

#endif