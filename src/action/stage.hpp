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

#ifndef ACTION_STAGE_HPP
#define ACTION_STAGE_HPP

#include "cup/stage_data.hpp"

namespace ts
{
    namespace world
    {
        class World;
        class Car;
    }

    namespace controls
    {
        class Control_center;
    }

    namespace action
    {
        using Stage_data = cup::Stage_data;
        using Car_data = cup::Car_data;

        // The Stage class keeps information about cars, their controllers, and various other things.
        // It adopts the World object, because this class should be the public interface to all
        // stage-related things.
        class Stage
        {
        public:
            Stage(std::unique_ptr<world::World> world, const Stage_data& stage_data);
            ~Stage();

            Stage_data stage_data() const;

            const world::World& world() const;
            const controls::Control_center& control_center() const;

            struct Internal_car_data
                : public Car_data
            {
                world::Car* car;
            };

            const std::vector<Internal_car_data>& car_data() const;

            void update(std::size_t frame_duration);
            void launch_game();


        private:
            void create_stage_entities(const Stage_data& stage_data);

            std::vector<Internal_car_data> car_data_;
            std::unordered_map<std::uint16_t, world::Car*> car_lookup_;

            resources::Track_handle track_handle_;

            std::unique_ptr<world::World> world_;
            std::unique_ptr<controls::Control_center> control_center_;

            
        };
    }
}


#endif