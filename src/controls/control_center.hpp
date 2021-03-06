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

#ifndef CONTROLS_CONTROL_CENTER_HPP
#define CONTROLS_CONTROL_CENTER_HPP

#include "control.hpp"

namespace ts
{
    namespace controls
    {
        class Controllable;
        struct Control_event;

        // class Control_center
        // Maps input slots to Controllable objects. This is used for identifying which controllable
        // object belongs to a local player.
        class Control_center
        {
        public:
            // Harbinger-style function naming.
            void assume_control(Slot slot, Controllable* controllable);
            void handle_control_event(const Control_event& event) const;

            void toggle_global_control(Control control, bool enable);

            Slot get_controllable_slot(const Controllable* controllable) const;
            bool is_controlled(const Controllable* controllable) const;

            const std::vector<const Controllable*>& get_controllables_by_slot(Slot slot) const;

        private:
            std::multimap<Slot, Controllable*> control_mapping_;
            std::uint32_t globally_disabled_controls_ = 0;

            mutable std::vector<const Controllable*> controllable_list_;
        };
    }
}

#endif