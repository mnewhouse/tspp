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

#ifndef WORLD_COLLISION_HELPERS_HPP
#define WORLD_COLLISION_HELPERS_HPP

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
        Vector2<double> get_edge_normal(Vector2i point, Vector2<double> heading, WallTest is_wall);

        Vector2<double> adjust_normal_to_heading(Vector2<double> normal, Vector2<double> heading);
        Vector2<double> adjust_normal_to_rotation(Vector2<double> normal, Rotation<double> rotation);

        template <typename CollisionTest>
        auto test_trajectory(Vector2<double> point, Vector2<double> end, CollisionTest test_func) ->
            ts::world::Trajectory_point<typename std::decay<decltype(test_func(point, 0.0))>::type>;
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

    if (it_a == wall_test_order.end() || it_b == wall_test_order.rend() || *it_a == *it_b)
    {
        return -heading;
    }

    auto normal = normalize(Vector2<double>(it_a->y - it_b->y, it_b->x - it_a->x));
    return dot_product(normal, -heading) >= dot_product(-normal, -heading) ? normal : -normal;
}

inline ts::Vector2<double> ts::world::adjust_normal_to_heading(Vector2<double> normal, Vector2<double> heading)
{
    auto rotation = Rotation<double>::radians(std::atan2(heading.x, -heading.y));
    return adjust_normal_to_rotation(normal, rotation);
}

inline ts::Vector2<double> ts::world::adjust_normal_to_rotation(Vector2<double> normal, Rotation<double> rotation)
{
    auto normal_angle = std::atan2(normal.x, -normal.y);
    auto deviation = std::fmod(rotation.radians() - normal_angle, 1.570796326794896557998981734272);

    // This is pretty abysmal, with all these hardcoded rotations. I'm fully aware of that.

    if (deviation < 0.0)
    {
        deviation += 1.570796326794896557998981734272;
    }
    
    if (deviation < 0.23182380450040304675773938924976391717791557312012)
    {
        normal_angle += deviation;
    }
    
    else if (deviation < 0.624522886199127214013060438446700572967529296875)
    {
        normal_angle += deviation - 0.4636476090008060935154787784;
    }
    
    else if (deviation < 0.9462734405957693439859212958253920078277587890625)
    {
        normal_angle += deviation - 0.7853981633974482789994908671;
    }
    
    else if (deviation < 1.3389725222944934834856667293934151530265808105469)
    {
        normal_angle += deviation - 1.1071487177940904089723517245;
    }
    
    else {
        normal_angle += deviation - 1.5707963267948965579989817342;
    }
    
    normal.x = std::sin(normal_angle);
    normal.y = -std::cos(normal_angle);

    return normal;
}


#endif
