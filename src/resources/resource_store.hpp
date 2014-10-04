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

#ifndef RESOURCES_RESOURCE_STORE_HPP
#define RESOURCES_RESOURCE_STORE_HPP

namespace ts
{
    namespace resources
    {
        class Car_store;
        class Track_store;
        class Player_store;
        class Script_manager;
        class Font_library;

        struct Settings;

        class Video_settings;
        class Car_settings;
        class Track_settings;
        class Script_settings;
        class Input_settings;
        class Player_settings;
        class Network_settings;
        class Audio_settings;

        struct Resource_store
        {
            Resource_store();
            ~Resource_store();

            Resource_store(const Resource_store&) = delete;
            Resource_store& operator=(const Resource_store&) = delete;

            Car_store& car_store();
            Track_store& track_store();
            Player_store& player_store();
            Script_manager& script_manager();
            Settings& settings();
            Font_library& font_library();

            const Car_store& car_store() const;
            const Track_store& track_store() const;
            const Player_store& player_store() const;
            const Script_manager& script_manager() const;
            const Settings& settings() const;
            const Font_library& font_library() const;

            const Car_settings& car_settings() const;
            Car_settings& car_settings();

            const Video_settings& video_settings() const;
            Video_settings& video_settings();

            const Track_settings& track_settings() const;
            Track_settings& track_settings();

            const Script_settings& script_settings() const;
            Script_settings& script_settings();

            const Input_settings& input_settings() const;
            Input_settings& input_settings();

            const Player_settings& player_settings() const;
            Player_settings& player_settings();

            const Network_settings& network_settings() const;
            Network_settings& network_settings();

            const Audio_settings& audio_settings() const;
            Audio_settings& audio_settings();

        private:
            std::unique_ptr<Car_store> car_store_;
            std::unique_ptr<Track_store> track_store_;
            std::unique_ptr<Player_store> player_store_;
            std::unique_ptr<Script_manager> script_manager_;
            std::unique_ptr<Settings> settings_;
            std::unique_ptr<Font_library> font_library_;
        };
    }
}

#endif