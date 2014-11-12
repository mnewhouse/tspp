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

#ifndef CUP_LISTENER_HPP
#define CUP_LISTENER_HPP

#include "cup_metadata.hpp"

namespace ts
{
    namespace resources
    {
        struct Track_metadata;
    }

    namespace cup
    {
        struct Stage_data;

        struct Cup_listener
        {
            virtual void on_state_change(Cup_state old_state, Cup_state new_state) {}
            virtual void on_restart() {}
            virtual void on_end() {}

            virtual void on_initialize(const Stage_data& stage_data) {}

        protected:
            virtual ~Cup_listener() = default;
        };
    }
}

#endif