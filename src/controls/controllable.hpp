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

#ifndef CONTROL_CONTROLLABLE_HPP
#define CONTROL_CONTROLLABLE_HPP

#include "control.hpp"

namespace ts
{

    namespace controls
    {
        // Controllable
        // Base class. This interface allows objects of derived types to exhibit various traits
        // based on which controls are set, allowing for a flexible control system.
        class Controllable
        {
        public:
            Controllable() = default;

            void set_control_state(Control control, bool state);
            bool control_state(Control control) const;
            
            std::uint32_t control_state() const;

            void set_controllable_id(std::uint16_t controllable_id);
            std::uint16_t controllable_id() const;


        private:
            std::uint32_t control_state_ = 0;
            std::uint16_t controllable_id_ = 0;
        };

    }

}

#endif