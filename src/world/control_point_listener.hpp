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

#ifndef WORLD_CONTROL_POINT_LISTENER_HPP
#define WORLD_CONTROL_POINT_LISTENER_HPP

#include "resources/control_point.hpp"

namespace ts
{
    namespace world
    {
        using resources::Control_point;
        class Entity;

        struct Control_point_listener
        {
            virtual void on_control_point_hit(Entity* entity, const Control_point* control_point, double time_point) {}
            virtual void on_control_area_enter(Entity* entity, const Control_point* control_point, double time_point) {}
            virtual void on_control_area_leave(Entity* entity, const Control_point* control_point, double time_point) {}
        };
    }
}


#endif