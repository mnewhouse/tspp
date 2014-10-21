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

#ifndef CUP_HPP
#define CUP_HPP

#include "cup_metadata.hpp"

#include "resources/track_handle.hpp"
#include "resources/car_handle.hpp"

#include "resources/settings/car_settings.hpp"
#include "resources/settings/track_settings.hpp"

namespace ts
{
    namespace resources
    {
        struct Car_definition;
    }

    namespace cup
    {
        struct Cup_listener;
        struct Stage_data;

        enum class Locality
        {
            Local,
            Remote
        };

        class Cup
        {
        public:
            Cup(Locality locality);
            ~Cup();

            void add_track(resources::Track_handle track_handle);
            void remove_track(resources::Track_handle track_handle);
            void clear_tracks();

            const std::vector<resources::Track_handle>& track_list() const;         

            void select_car(resources::Car_handle car_handle);
            void set_car_mode(resources::Car_mode car_mode);

            const std::vector<resources::Car_handle>& car_list() const;
            resources::Car_mode car_mode() const;

            void add_cup_listener(Cup_listener* cup_listener);
            void remove_cup_listener(Cup_listener* cup_listener);

            bool is_local() const;
            Cup_state cup_state() const;
            std::size_t cup_progress() const;

            void set_cup_state(Cup_state cup_state);
            void set_cup_progress(std::size_t index);

            void load_car_settings(const resources::Car_settings& car_settings);
            void load_track_settings(const resources::Track_settings& track_settings);

            const resources::Car_settings& car_settings() const;
            const resources::Track_settings& track_settings() const;

            void end();
            void restart();
            void advance();

            void initialize_action(const Stage_data& stage_data);

            resources::Track_handle current_track() const;

            Player_handle add_player(const Player& player, Player_id player_id);
            Player_handle add_player(const Player& player);
            void remove_player(Player_handle player_handle);
            void clear_players();

            const std::vector<Player_handle>& player_list() const;
            const std::vector<Player_handle>& action_players() const;
            const std::vector<Player_handle>& local_players() const;

            std::size_t player_count() const;
            std::size_t max_players() const;
            Player_handle get_player_by_id(Player_id player_id) const;
            

            void set_player_car(Player_handle player, resources::Car_handle car_handle);            

        private:
            void launch_action();
            void stop_action();
            void preinitialize_action();
            void initialize_action();
            void start_cup();

            Player_id allocate_player_id() const;

            Cup_state state_;
            Locality locality_;

            std::uint32_t max_players_;

            resources::Track_settings track_settings_;
            resources::Car_settings car_settings_;

            //std::vector<resources::Script_handle> loaded_scripts_;

            std::size_t cup_progress_;
            std::map<Player_id, Cup_player_data> player_map_;
            std::vector<Player_handle> player_list_;
            std::vector<Player_handle> action_players_;
            std::vector<Player_handle> local_players_;

            std::vector<Cup_listener*> cup_listeners_;
        };
    }
}

#endif
