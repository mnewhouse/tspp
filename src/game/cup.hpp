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

#ifndef GAME_CUP_HPP
#define GAME_CUP_HPP

#include "stage_data.hpp"
#include "player.hpp"

#include "resources/resource_store.hpp"
#include "resources/settings_copy.hpp"

namespace ts
{
    namespace resources
    {
        class Track_handle;
        struct Car_definition;
    }

    namespace game
    {
        struct Cup_listener;

        enum class Cup_type
        {
            Local,
            Server,
            Remote
        };

        enum class Cup_state
        {
            Registering,
            Cup,
            Car_selection,
            Initializing,
            Action,
            End
        };

        using Player_id = std::uint8_t;

        struct Cup_player_data
        {
            Player_id player_id;
            Player player;
            Car_data car_data;
            bool ready_for_action = false;
        };

        class Cup
        {
        public:
            using Player_handle = Pointer_handle<const Cup_player_data>;

            Cup(game::Cup_type cup_type, resources::Resource_store* resource_store);

            void add_track(resources::Track_handle track_handle);
            const std::vector<resources::Track_handle>& track_list() const;            

            const std::vector<resources::Car_handle>& car_list() const;
            resources::Car_mode car_mode() const;

            void add_cup_listener(Cup_listener* cup_listener);
            void remove_cup_listener(Cup_listener* cup_listener);

            Cup_type cup_type() const;
            Cup_state cup_state() const;
            
            void advance();
            void initialize_action();

            void end();
            void restart();

            void signal_ready();
            void signal_ready(Player_handle player_handle);

            resources::Track_handle current_track() const;

            Player_handle add_local_player(const Player& player, controls::Slot player_slot);
            Player_handle add_remote_player(const Player& player);

            std::size_t player_count() const;
            const std::vector<Player_handle>& local_players() const;

            void set_player_car(Player_handle, resources::Car_handle car_handle);

            Stage_data make_stage_data() const;

        private:
            void launch_if_ready();

            void assign_start_positions();
            void change_state(Cup_state new_state);
            void preinitialize_action();
            void launch_action();
            void stop_action();
            void start_cup();
            Player_id allocate_player_id() const;

            Cup_type cup_type_;
            Cup_state state_;

            resources::Resource_store* resource_store_;

            resources::Settings_copy<resources::Car_settings> car_settings_;
            resources::Settings_copy<resources::Track_settings> track_settings_;
            resources::Settings_copy<resources::Script_settings> script_settings_;

            std::vector<resources::Script_handle> loaded_scripts_;
            std::vector<Player_handle> local_players_;
            std::uint32_t not_ready_count_ = 0;

            std::size_t current_track_index_;
            std::map<Player_id, Cup_player_data> player_map_;

            std::vector<Cup_listener*> cup_listeners_;
        };

    }

}

#endif
