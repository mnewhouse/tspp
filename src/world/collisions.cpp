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

#include "collisions.hpp"

#include <algorithm>

namespace ts
{
    namespace world
    {
        template <typename WallChecker>
        Collision_point find_wall_between_points(Vector2i first_point, Vector2i second_point, WallChecker is_wall);

        template <typename WallTest>
        Vector2d get_edge_normal(const resources::Pattern& pattern, WallTest is_wall, Vector2i point, Vector2i origin);
    }
}


template <typename WallChecker>
ts::world::Collision_point ts::world::find_wall_between_points(Vector2i first_point, Vector2i second_point, WallChecker is_wall)
{
    int dx = std::abs(second_point.x - first_point.x);
    int dy = std::abs(second_point.y - first_point.y);

    auto collided = [=](Vector2i point)
    {
        Collision_point result;
        result.collided = true;
        result.global_point = point;

        result.time_point = 1.0;

        if (dx != 0 && dy != 0) {
            auto px = double(point.x - first_point.x) / double(second_point.x - first_point.x);
            auto py = double(point.y - first_point.y) / double(second_point.y - first_point.y);
            result.time_point = (px + py) * 0.5;
        }

        else if (dy != 0) {            
            result.time_point = (point.y - first_point.y) / double(second_point.y - first_point.y);
        }

        else if (dx != 0) {
            result.time_point = double(point.x - first_point.x) / double(second_point.x - first_point.x);
        }

        return result;
    };

    int sx = first_point.x < second_point.x ? 1 : -1;
    int sy = first_point.y < second_point.y ? 1 : -1;
    int e = dx - dy;

    while (first_point.x != second_point.x || first_point.y != second_point.y)
    {
        int e2 = e << 1;

        if (e2 > -dy) {
            if (is_wall(first_point)) return collided(first_point);

            e -= dy;
            first_point.x += sx;
        }

        if (first_point.x != second_point.x || first_point.y != second_point.y) {
            if (e2 < dx) {
                if (is_wall(first_point)) return collided(first_point);

                e += dx;
                first_point.y += sy;
            }
        }
    }

    if (is_wall(first_point)) return collided(first_point);

    return{ false };
}

template <typename WallTest>
ts::Vector2d ts::world::get_edge_normal(const resources::Pattern& pattern, WallTest is_wall, Vector2i point, Vector2i origin)
{
    auto offset = point - origin;
    auto dx = offset.x > 0 ? 1 : -1;
    auto dy = offset.y > 0 ? 1 : -1;

    if (offset.x != 0) {

        Vector2i pt_a{ point.x - dx, point.y - 1 };
        Vector2i pt_b{ point.x - dx, point.y + 1 };

        if (!is_wall(pt_a)) {
            pt_a.x += dx;
            if (!is_wall(pt_a)) pt_a.x += dx;
        }

        if (!is_wall(pt_b)) {
            pt_b.x += dx;
            if (!is_wall(pt_b)) pt_b.x += dx;
        }

        if (is_wall(pt_a) && is_wall(pt_b)) {
            auto intersect_x = pt_a.x - (((origin.y - pt_a.y) * (pt_a.x - pt_b.x)) >> 1);
            auto intersect_y = pt_a.y - (((origin.x - pt_a.x) * (pt_a.x - pt_b.x)) << 1);

            Vector2i normal{ std::abs(pt_b.y - pt_a.y), std::abs(pt_b.x - pt_a.x) };

            if (origin.x < intersect_x) normal.x = -normal.x;
            if (origin.y < intersect_y) normal.y = -normal.y;

            return normal;
        }
    }

    if (offset.y != 0) {
        Vector2i pt_a{ point.x - 1, point.y - dy, };
        Vector2i pt_b{ point.x + 1, point.y - dy };

        if (!is_wall(pt_a)) {
            pt_a.y += dy;
            if (!is_wall(pt_a)) pt_a.y += dy;
        }

        if (!is_wall(pt_b)) {
            pt_b.y += dy;
            if (!is_wall(pt_b)) pt_b.y += dy;
        }

        if (is_wall(pt_a) && is_wall(pt_b)) {
            auto intersect_x = pt_a.x - (((origin.y - pt_a.y) * (pt_a.y - pt_b.y)) << 1);
            auto intersect_y = pt_a.y - (((origin.x - pt_a.x) * (pt_a.y - pt_b.y)) >> 1);            

            Vector2i normal{ std::abs(pt_b.y - pt_a.y), std::abs(pt_b.x - pt_a.x) };

            if (origin.x < intersect_x) normal.x = -normal.x;
            if (origin.y < intersect_y) normal.y = -normal.y;

            return normal;
        }
    }

    if (offset.x != 0) {
        Vector2i pt{ point.x - dx, point.y - dy };
        if (!is_wall(pt)) {
            pt.x += dx;
            if (!is_wall(pt)) pt.x += dx;
        }

        if (!is_wall(pt)) {
            pt = { point.x - dx, point.y + dy };
            if (!is_wall(pt)) {
                pt.x += dx;
                if (!is_wall(pt)) pt.x += dx;
            }
        }

        if (is_wall(pt)) {
            auto intersect_y = point.y + ((origin.x - point.x) * (pt.x - point.x) * (pt.y - point.y));
            auto intersect_x = point.x + ((origin.y - point.y) * (pt.x - point.x) * (pt.y - point.y));

            Vector2i normal{ std::abs(pt.y - point.y), std::abs(pt.x - point.x) };

            if (origin.x < intersect_x) normal.x = -normal.x;
            if (origin.y < intersect_y) normal.y = -normal.y;

            return normal;
        }
    }


    if (offset.y != 0) {
        Vector2i pt{ point.x - dx, point.y - dy };
        if (!is_wall(pt)) {
            pt.y += dy;
            if (!is_wall(pt)) pt.y += dy;
        }

        if (!is_wall(pt)) {
            pt = { point.x + dx, point.y - dy};
            if (!is_wall(pt)) {
                pt.y += dy;
                if (!is_wall(pt)) pt.y += dy;
            }
        }

        if (is_wall(pt)) {
            auto intersect_x = point.x + ((origin.y - point.y) * (pt.y - point.y) * (pt.x - point.x));
            auto intersect_y = point.y + ((origin.x - point.x) * (pt.y - point.y) * (pt.x - point.x));

            Vector2i normal{ std::abs(pt.y - point.y), std::abs(pt.x - point.x) };

            if (origin.x < intersect_x) normal.x = -normal.x;
            if (origin.y < intersect_y) normal.y = -normal.y;

            return normal;
        }
    }

    if (std::abs(offset.y) > std::abs(offset.x)) {
        return Vector2d{ 0.0, offset.y > 0 ? 1.0 : -1.0 };
    }
        
    return Vector2d{ offset.x > 0 ? 1.0 : -1.0, 0.0 };
}


ts::world::Collision_result ts::world::detect_collision(const Entity& entity, Vector2i new_position, Rotation<double> new_rotation,
                                  const resources::Pattern& scenery, const resources::Terrain_library& terrain_lib)
{
    auto old_rotation = entity.rotation();
    Vector2i old_position = entity.position();

    auto z_level = static_cast<int>(entity.z_position());

    const auto& collision_mask = entity.collision_mask();
    const auto& bounding_box = collision_mask.bounding_box;

    Collision_result result;
    result.collided = false;
    result.stuck = false;

    if (!collision_mask.pattern) {
        return result;
    }

    const auto& pattern = *collision_mask.pattern;

    auto old_cos = long(std::cos(old_rotation.radians()) * 65536.0 + 0.5);
    auto old_sin = long(std::sin(old_rotation.radians()) * 65536.0 + 0.5);

    auto new_cos = long(std::cos(new_rotation.radians()) * 65536.0 + 0.5);
    auto new_sin = long(std::sin(new_rotation.radians()) * 65536.0 + 0.5);

    auto offset = new_position - old_position;

    Vector2i center{ bounding_box.left + (bounding_box.width >> 1),
        bounding_box.top + (bounding_box.height >> 1) };

    Vector2i scenery_size = scenery.size();

    auto wall_test = [&scenery, &terrain_lib, scenery_size, z_level](Vector2i point)
    {
        if (point.x < 0 || point.x >= scenery_size.x || point.y < 0 || point.y >= scenery_size.y) return false;

        auto terrain_id = scenery(point.x, point.y);
        terrain_id = terrain_lib.sub_terrain(terrain_id, z_level);

        return terrain_lib.terrain_by_id(terrain_id).is_wall;
    };

    for (auto y = bounding_box.top; y != bounding_box.bottom(); ++y) {
        for (auto x = bounding_box.left; x != bounding_box.right(); ++x) {
            if (pattern(x, y) == 0) continue;

            Vector2i point{ x, y };
            point -= center;

            Vector2i old_transformed{ (old_cos * point.x - old_sin * point.y + 0x8000) >> 16,
                (old_sin * point.x + old_cos * point.y + 0x8000) >> 16 };

            Vector2i new_transformed{ (new_cos * point.x - new_sin * point.y + 0x8000) >> 16,
                (new_sin * point.x + new_cos * point.y + 0x8000) >> 16 };

            old_transformed += old_position;
            new_transformed += new_position;

            if (wall_test(old_transformed)) {
                // Car in wall, this is ass
                result.stuck = true;
                result.collided = true;
                return result;
            }

            auto collision_point = find_wall_between_points(old_transformed, new_transformed, wall_test);

            if (collision_point.collided) {
                if (!result.collided || collision_point.time_point < result.time_point) {

                    result.collided = true;
                    result.normal = get_edge_normal(scenery, wall_test, collision_point.global_point, old_transformed);
                    result.time_point = collision_point.time_point;

                    Vector2i adjustment{};

                    if (result.normal.x < 0) adjustment.x = -1;
                    else if (result.normal.x > 0) adjustment.x = 1;

                    if (result.normal.y < 0) adjustment.y = -1;
                    else if (result.normal.y > 0) adjustment.y = 1;                    
                    
                    result.position.x = old_position.x + std::round(offset.x * collision_point.time_point) + adjustment.x;
                    result.position.y = old_position.y + std::round(offset.y * collision_point.time_point) + adjustment.y;

                    if (offset.x < 0 && result.position.x > old_position.x) result.position.x = old_position.x;
                    if (offset.x > 0 && result.position.x < old_position.x) result.position.x = old_position.x;

                    if (offset.y < 0 && result.position.y > old_position.y) result.position.y = old_position.y;
                    if (offset.y > 0 && result.position.y < old_position.y) result.position.y = old_position.y;

                    // Put the entity in the middle of the pixel, to keep things even.
                    result.position.x += 0.5;
                    result.position.y += 0.5;
                }
            }
        }
    }

    return result;
}