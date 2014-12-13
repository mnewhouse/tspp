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
            : public core::Listener_host<Cup_listener>
        {
        public:
            Cup(Locality locality);
            ~Cup();

            bool is_local() const;
            Cup_state cup_state() const;
            std::size_t cup_progress() const;
            std::size_t stage_count() const;

            void set_cup_state(Cup_state cup_state);
            void set_cup_progress(std::size_t index);
            void set_stage_count(std::size_t stage_count);
            

            void initialize_action(const Stage_data& stage_data);
            void initiate_car_selection();

            Player_handle add_player(const Player& player, Player_id player_id);
            Player_handle add_player(const Player& player);
            void remove_player(Player_handle player_handle);
            void clear_players();

            const std::vector<Player_handle>& player_list() const;
            const std::vector<Player_handle>& action_players() const;
            const std::vector<Player_handle>& local_players() const;

            std::size_t player_count() const;
            Player_handle get_player_by_id(Player_id player_id) const;            

        private:
            Player_id allocate_player_id() const;

            Cup_state state_;
            Locality locality_;

            /*
            resources::Track_settings track_settings_;
            resources::Car_settings car_settings_;
            */

            //std::vector<resources::Script_handle> loaded_scripts_;

            std::size_t cup_progress_ = 0;
            std::size_t stage_count_ = 0;
            std::map<Player_id, Player_definition> player_map_;
            std::vector<Player_handle> player_list_;
            std::vector<Player_handle> action_players_;
            std::vector<Player_handle> local_players_;
        };
    }
}

#endif
