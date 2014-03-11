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

#ifndef WORLD_COLLISION_HELPERS_HPP
#define WORLD_COLLISION_HELPERS_HPP

#include "core/vector2.hpp"
#include "core/rotation.hpp"

#include <cmath>
#include <array>
#include <utility>

namespace ts
{
    namespace world
    {
        template <typename ResultType>
        struct Trajectory_point
        {
            ResultType result;

            double time_point;
            double valid_time_point;    

            Vector2<double> point;
            Vector2<double> valid_point;
        };
        
        template <typename WallTest>
        ts::Vector2<double> get_edge_normal(Vector2i point, Vector2<double> heading, WallTest is_wall);

        template <typename CollisionTest>
        auto test_trajectory(Vector2<double> point, Vector2<double> end, CollisionTest test_func) ->
            ts::world::Trajectory_point<typename std::decay<decltype(test_func(point, 0.0))>::type>;

        inline Vector2<double> interpolate_position(const Vector2<double>& old_pos, const Vector2<double>& new_pos, double time_point)
        {
            return{ old_pos.x + ((new_pos.x - old_pos.x) * time_point),
                old_pos.y + ((new_pos.y - old_pos.y) * time_point) };
        }
    }
}


template <typename CollisionTest>
auto ts::world::test_trajectory(Vector2<double> start, Vector2<double> end, CollisionTest test_func) ->
    ts::world::Trajectory_point<typename std::decay<decltype(test_func(start, 0.0))>::type>
{
    using result_type = typename std::decay<decltype(test_func(start, 0.0))>::type;

    Trajectory_point<result_type> result;
    result.point = start;
    result.valid_point = start;
    result.time_point = 0.0;
    result.valid_time_point = -1.0;

    auto offset = end - start;
    auto frame_count = std::max(std::abs(int(offset.x)), std::abs(int(offset.y))) + 1;

    auto time_step = 1.0 / frame_count;

    for (auto frame = 0; frame <= frame_count; ++frame)
    {
        result.point = interpolate_position(start, end, result.time_point);

        result.result = test_func(result.point, result.time_point);
        if (result.result) return result;

        result.valid_point = result.point;
        result.valid_time_point = result.time_point;

        result.time_point += time_step;
    }

    return result;
}

#ifdef DISABLED
template <typename CollisionTest>
auto ts::world::test_trajectory(Vector2i point, Vector2i end, CollisionTest test_func) ->
    ts::world::Trajectory_point<typename std::decay<decltype(test_func(point, 0.0))>::type>
{
    using result_type = typename std::decay<decltype(test_func(point, 0.0))>::type;

    Trajectory_point<result_type> result;
    result.point = point;
    result.valid_point = point;
    result.time_point = 0.0;
    result.valid_time_point = -1.0;

    const auto dx = std::abs(result.point.x - end.x);
    const auto dy = std::abs(point.y - end.y);

    const auto sx = result.point.x < end.x ? 1 : -1;
    const auto sy = result.point.y < end.y ? 1 : -1;

    auto e = dx - dy;

    const double time_step = (dx || dy ? 1.0 / std::max(dx, dy) : 0.0);

    while (result.point != end)
    {
        result.result = test_func(result.point, result.time_point);
        if (result.result) return result;

        result.valid_point = result.point;        
        result.valid_time_point = result.time_point;        

        auto e2 = e * 2;
        if (e2 > -dy) 
        {
            e -= dy;
            result.point.x += sx;
        }

        else
        {
            e += dx;
            result.point.y += sy;
        }

        result.time_point += time_step;
    }

    result.result = test_func(result.point, 1.0);
    return result;
}
#endif

template <typename WallTest>
ts::Vector2<double> ts::world::get_edge_normal(Vector2i point, Vector2<double> heading, WallTest is_wall)
{
    auto wall_test_order = [heading]()-> std::array<Vector2i, 7>
    {
        bool steep = std::abs(heading.y) > std::abs(heading.x);

        if (heading.x >= 0.0)
        {
            if (heading.y >= 0.0)
            {
                if (steep)
                    return { { { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 0 }, { 1, 0 }, { 1, -1 }, { 0, -1 } } };

                return { { { -1, 0 }, { -1, 1 }, { 0, 1 }, { 0, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 } } };
            }

            else if (steep)
                return { { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 }, { -1, -1 }, { -1, 0 }, { -1, 1 } } };

            return { { { -1, 1 }, { 0, 1 }, { 1, 1 }, { 0, 0 }, { 0, -1 }, { -1, -1 }, { -1, 0 } } };
        }

        if (heading.y >= 0.0)
        {
            if (steep)
                return { { { 0, -1 }, { -1, -1 }, { -1, 0 }, { 0, 0 }, { 1, 1 }, { 1, 0 }, { 1, -1 } } };

            return{ { { 1, -1 }, { 0, -1 }, { -1, -1 }, { 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 0 } } };            
        }

        else if (!steep)
            return { { { 1, 1 }, { 0, 1 }, { -1, 1 }, { 0, 0 }, { 0, -1 }, { 1, -1 }, { 1, 0 } } };

        return { { { 0, 1 }, { -1, 1 }, { -1, 0 }, { 0, 0 }, { 1, -1 }, { 1, 0 }, { 1, 1 } } };
    }();

    auto predicate = [&](Vector2i offset)
    {
        return is_wall(point + offset);
    };

    auto it_a = std::find_if(wall_test_order.begin(), wall_test_order.end(), predicate);
    auto it_b = std::find_if(wall_test_order.rbegin(), wall_test_order.rend(), predicate);

    heading = normalize(heading);

    if (it_a == wall_test_order.end() || it_b == wall_test_order.rend())
    {
        return -heading;
    }

    auto normal = normalize(Vector2<double>(it_a->y - it_b->y, it_b->x - it_a->x));
    return dot_product(normal, -heading) >= dot_product(-normal, -heading) ? normal : -normal;
}



#endif
