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

#ifndef STAGE_ASSEMBLER_HPP
#define STAGE_ASSEMBLER_HPP

#include "cup/cup_metadata.hpp"

namespace ts
{
    namespace resources
    {
        struct Player_definition;
        struct Car_model;
    }

    namespace cup
    {
        class Cup_controller;
        struct Stage_data;
    }

    namespace server
    {
        class Stage_assembler
        {
        public:
            Stage_assembler(cup::Cup_controller* cup_controller);
            ~Stage_assembler();

            const cup::Stage_data& initialize_stage_data(const cup::Cup_controller* cup_controller);

            std::uint32_t add_car(const resources::Player_definition& player_definition, const resources::Car_model& car_model,
                                  cup::Player_handle controller = cup::Player_handle(), std::int32_t start_pos = -1);

            void remove_car_by_id(std::uint32_t car_id);

            const cup::Stage_data& stage_data() const;

            using Modifier = std::function<void(Stage_assembler&)>;
            using Modifier_handle = std::list<Modifier>::const_iterator;

            Modifier_handle add_modifier(Modifier modifier);
            void remove_modifier(Modifier_handle handle);           

        private:
            struct Impl;
            std::unique_ptr<Impl> impl_;
        };
    }
}

#endif