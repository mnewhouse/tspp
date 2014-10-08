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

#include "cup/stage_data.hpp"

#include "controls/control_center.hpp"

namespace ts
{
    namespace cup
    {
        class Cup;
    }

    namespace world
    {
        class World;
    }

    namespace game
    {
        // function assemble_stage
        // Gather the stage information for a given cup.
        cup::Stage_data assemble_stage(const cup::Cup& cup);

        // function create_stage_entities
        // Spawn the stage entities into |world| according to the information in |stage_data|.
        void create_stage_entities(world::World* world, const cup::Stage_data& stage_data);

        // function create_control_center
        // Create a control center that assumes control of all the controlled entities in |stage_data|
        std::unique_ptr<controls::Control_center> create_control_center(const world::World* world, const cup::Stage_data& stage_data);
    }
}

#endif