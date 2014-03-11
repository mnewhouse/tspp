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

#ifndef GRAPHICS_FPS_COUNTER_HPP
#define GRAPHICS_FPS_COUNTER_HPP

#include <chrono>

namespace ts
{

    namespace graphics
    {

        class FPS_Counter
        {
        public:
            FPS_Counter();
            FPS_Counter& operator++();

            double current_fps() const;
        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> time_point_;
            std::size_t count_;
            double last_fps_;
        };

    }

}

#endif