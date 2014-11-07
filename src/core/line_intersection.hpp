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

#ifndef LINE_INTERSECTION_HPP
#define LINE_INTERSECTION_HPP

#include "vector2.hpp"

namespace ts
{
    template <typename T>
    bool line_intersection(Vector2<T> a1, Vector2<T> a2, Vector2<T> b1, Vector2<T> b2, 
                           Vector2<T>& result)
    {
        auto d = (a1.x - a2.x) * (b1.y - b2.y) - (a1.y - a2.y) * (b1.x - b2.x);
        if (d == 0) return false;

        auto a = (a1.x * a2.y - a1.y * a2.x);
        auto b = (b1.x * b2.y - b1.y * b2.x);

        result.x = ((b1.x - b2.x) * a - (a1.x - a2.x) * b) / d;
        result.y = ((b1.y - b2.y) * a - (a1.y - a2.y) * b) / d;    
        return true;
    }
}

#endif