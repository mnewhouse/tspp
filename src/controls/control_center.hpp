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

#ifndef CONTROLS_CONTROL_CENTER_HPP
#define CONTROLS_CONTROL_CENTER_HPP

#include "control.hpp"

#include <map>

namespace ts
{
    namespace controls
    {
        class Controllable;

        // class Control_center
        // Maps input slots to Controllable objects. This is used for identifying which controllable
        // object belongs to a local player.
        class Control_center
        {
        public:
            // Harbinger-style function naming.
            void assume_control(Slot slot, Controllable* controllable);

            void update_control_state(Slot slot, unsigned int new_state);
            void set_control_state(Slot slot, Control control, bool state);
            

        private:
            std::multimap<int, Controllable*> control_mapping_;
        };
    }
}

#endif