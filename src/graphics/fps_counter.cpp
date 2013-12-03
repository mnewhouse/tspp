/*
 * Turbo Sliders++
 * Copyright (C) 2013 Martin Newhouse
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

#include "fps_counter.hpp"
#include <iostream>

ts::graphics::FPS_Counter::FPS_Counter()
    : time_point_(std::chrono::high_resolution_clock::now()), 
      count_(0), 
      last_fps_(0.0)
{
}

ts::graphics::FPS_Counter& ts::graphics::FPS_Counter::operator++()
{
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    auto elapsed_time = duration_cast<microseconds>(now - time_point_);
    auto usec = elapsed_time.count();

    if (usec >= 1000000) {
        last_fps_ = count_ / (usec * 1e-6);
        count_ = 0;
        time_point_ = now;
    }

    else {
        ++count_;
    }

    return *this;
}

double ts::graphics::FPS_Counter::current_fps() const
{
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    auto elapsed_time = duration_cast<microseconds>(now - time_point_);
    auto micro_seconds = elapsed_time.count();
    auto seconds = micro_seconds * 1e-6;

    if ((last_fps_ == 0.0 && seconds != 0.0) || micro_seconds >= 1000000) {
        return count_ / seconds;
    }

    else {
        return count_ + last_fps_ * (1.0 - seconds);
    }
}