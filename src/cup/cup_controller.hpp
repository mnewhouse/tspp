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

#ifndef CUP_CONTROLLER_HPP
#define CUP_CONTROLLER_HPP

#include "cup/cup_metadata.hpp"

#include "resources/settings/car_settings.hpp"
#include "resources/settings/track_settings.hpp"

namespace ts
{
    namespace resources
    {
        struct Resource_store;
    }

    namespace cup
    {
        class Cup;
        class Cup_config;
        struct Cup_listener;
        struct Stage_data;

        struct Stage_player
        {
            Player_handle player;
            resources::Car_handle car;
            std::uint32_t start_pos = 0;
        };

        class Cup_controller
        {
        public:
            Cup_controller(resources::Resource_store* resource_store);
            ~Cup_controller();

            const Cup* cup() const;

            void add_cup_listener(Cup_listener* listener);
            void remove_cup_listener(Cup_listener* listener);

            void advance();            
            cup::Cup_state cup_state() const;

            const std::vector<resources::Car_handle>& car_list() const;
            const resources::Car_settings& car_settings() const;
            resources::Car_mode car_mode() const;

            const std::vector<resources::Track_handle>& track_list() const;
            const resources::Track_settings& track_settings() const;
            resources::Track_handle current_track() const;

            std::size_t stage_count() const;
            std::size_t cup_progress() const;
            void skip_stage();

            resources::Car_handle player_car(const Player_handle& player_handle) const;
            void set_player_car(const Player_handle& player_handle, const resources::Car_handle& car_handle);

            Player_handle add_player(const Player& player);
            Player_handle add_player(const Player& player, Player_id player_id);

            void initialize_action(const cup::Stage_data& stage_data);
            const std::vector<Stage_player>& stage_players() const;

        private:
            void set_cup_state(cup::Cup_state cup_state);

            struct Impl;
            std::unique_ptr<Impl> impl_;
        };
    }
}

#endif