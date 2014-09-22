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

#ifndef CORE_TRANSFORM_HPP
#define CORE_TRANSFORM_HPP

namespace ts
{
    inline Vector2<double> interpolate_position(const Vector2<double>& old_pos, const Vector2<double>& new_pos, double time_point)
    {
        return{ old_pos.x + ((new_pos.x - old_pos.x) * time_point),
            old_pos.y + ((new_pos.y - old_pos.y) * time_point) };
    }

    inline Vector2<double> transform_point(const Vector2<double>& point, double sin, double cos)
    {
        return{ point.x * cos - sin * point.y, point.y * cos + sin * point.x };
    }

    inline Vector2<double> transform_point(const Vector2<double>& point, Rotation<double> rotation)
    {
        auto rad = rotation.radians();
        return transform_point(point, std::sin(rad), std::cos(rad));
    }

    inline Vector2i fast_transform_point(Vector2i point, std::int32_t sin, std::int32_t cos)
    {
        Vector2i result =
        {
            (point.x * cos - point.y * sin + 0x8000) >> 16,
            (point.y * cos + sin * point.x + 0x8000) >> 16
        };

        return result;
    }
}

#endif