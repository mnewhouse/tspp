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

#ifndef CORE_BRESENHAM_HPP
#define CORE_BRESENHAM_HPP

#include "vector2.hpp"

namespace ts
{
    template <typename Callback>
    void bresenham(Vector2i start, Vector2i end, Callback callback);
}

template <typename Callback>
void ts::bresenham(Vector2i start, Vector2i end, Callback callback)
{
    auto dx = std::abs(end.x - start.x);
    auto dy = std::abs(end.y - start.y);

    auto sx = (start.x < end.x ? 1 : -1);
    auto sy = (start.y < end.y ? 1 : -1);

    auto err = dx - dy;

    while (start != end)
    {
        callback(start);

        auto e2 = err * 2;
        if (e2 >= -dy)
        {
            err -= dy;
            start.x += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            start.y += sy;
        }
    }

    callback(end);
}

#endif
