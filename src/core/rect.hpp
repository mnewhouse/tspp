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

#ifndef RECT_HPP
#define RECT_HPP

#include "vector2.hpp"

#include <istream>

namespace ts
{

    template <typename T>
    struct Rect
    {
    public:
        Rect()
            : left(), top(), width(), height()
        {}

        template <typename T1, typename T2>
        Rect(Vector2<T1> point, Vector2<T2> size)
            : left(point.left), top(point.top), width(size.x), height(size.y)
        {}

        Rect(T left, T top, T width, T height)
            : left(left), top(top), width(width), height(height)
        {}

        T right() const { return left + width; }
        T bottom() const { return top + height; }

        T left;
        T top;
        T width;
        T height;
    };

    template <typename T, typename U>
    bool contains(Rect<T> rect, Vector2<U> point)
    {
        return point.x >= rect.left && point.y >= rect.top &&
            point.x < rect.right() && point.y < rect.bottom();
    }

    template <typename T, typename U>
    bool intersects(Rect<T> a, Rect<U> b)
    {
        return a.left < b.right() && a.right() > b.left &&
            a.top < b.bottom() && a.bottom() > b.top;
    }

    template <typename T, typename U>
    Rect<typename std::common_type<T, U>::type> intersection(Rect<T> a, Rect<U> b)
    {
        auto a_min_x = std::min(a.left, a.right());
        auto a_max_x = std::max(a.left, a.right());
        auto a_min_y = std::min(a.top, a.bottom());
        auto a_max_y = std::max(a.top, a.bottom());

        auto b_min_x = std::min(b.left, b.right());
        auto b_max_x = std::max(b.left, b.right());
        auto b_min_y = std::min(b.top, b.bottom());
        auto b_max_y = std::max(b.top, b.bottom());

        Rect<typename std::common_type<T, U>::type> result;
        result.left = std::max(a_min_x, b_min_x);
        result.top = std::max(a_min_y, b_min_y);

        auto right = std::min(a_max_x, b_max_x);
        auto bottom = std::min(a_max_y, b_max_y);

        if (result.left < right && result.top < bottom) {
            result.width = right - result.left;
            result.height = bottom - result.top;
        }

        else {
            result.left = 0;
            result.top = 0;
        }

        return result;
    }

    template <typename T>
    std::istream& operator>>(std::istream& stream, Rect<T>& rect)
    {
        return stream >> rect.left >> rect.top >> rect.width >> rect.height;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& stream, const Rect<T>& rect)
    {
        return stream << rect.left << ", " << rect.top << ", " << rect.width << ", " << rect.height;
    }

    typedef Rect<float> Float_rect;
    typedef Rect<double> Double_rect;
    typedef Rect<int> Int_rect;

}

#endif