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

#ifndef CONTROLS_CONTROL_HPP
#define CONTROLS_CONTROL_HPP

#include <cstdint>
#include <string>

namespace ts
{

    namespace controls
    {
        using Slot = int;

        enum class Control
            : std::uint32_t
        {
            None = 0,
            Left = 1,
            Right = 2,
            Accelerate = 4,
            Brake = 8,
            Fire = 16,
            Alt_fire = 32
        };

        Control control_from_string(const std::string& control_string);

    }

}

#endif