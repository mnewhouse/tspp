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

#ifndef GUI_ELEMENTS_RECTANGLE_REGION_HPP
#define GUI_ELEMENTS_RECTANGLE_REGION_HPP

#include "core/rect.hpp"
#include "core/vector2.hpp"

namespace ts
{

namespace gui
{

namespace elements
{

template <typename T>
class Rectangle_region
{
public:
    Rectangle_region(Rect<T> rect)
        : rect(rect)
    {}

    template <typename U>
    bool operator()(Vector2<U> point)
    {
        return contains(rect, point);
    }

    Rect<T> rect;
};

}

}

}

#endif