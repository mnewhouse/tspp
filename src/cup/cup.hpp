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

#ifndef CUP_CUP_HPP
#define CUP_CUP_HPP

#include <vector>
#include <memory>

#include "stage_data.hpp"
#include "player.hpp"

namespace ts
{
    namespace resources
    {
        class Track_handle;
        struct Car_definition;
    }


    namespace cup
    {
        class Cup
        {
        public:
            enum class Car_mode
            {
                Fixed,
                Random,
                Free
            };

            Cup();

            void add_player(const Player& player);
            std::size_t player_count() const;

            void add_track(resources::Track_handle track_handle);
            const std::vector<resources::Track_handle>& track_list() const;

            void set_car(const resources::Car_definition& car_def);
            void set_car_mode(Car_mode car_mode);
            void add_car(const resources::Car_definition& car_def);

            const std::vector<resources::Car_definition>& car_list() const;
            Car_mode car_mode() const;

            void next_track();
            resources::Track_handle current_track() const;

            void make_stage_data();
            const Stage_data& stage_data() const;

        private:
            std::size_t current_track_;
            std::vector<resources::Track_handle> track_list_;
            std::vector<resources::Car_definition> car_list_;
            Car_mode car_mode_;

            std::vector<Player> player_list_;            

            Stage_data stage_data_;
        };

    }

}

#endif
