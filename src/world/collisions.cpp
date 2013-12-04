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


ts::world::Collision_result ts::world::detect_collision(const Entity_state& subject, const resources::Pattern& scenery, const resources::Terrain_library& terrain_lib)
{
    const auto& entity = *subject.entity;
    Vector2i new_position = subject.position;
    const auto& new_rotation = subject.rotation;

    auto old_rotation = entity.rotation();
    Vector2i old_position = entity.position();

    auto z_level = entity.z_level();

    const auto& collision_mask = entity.collision_mask();
    const auto& bounding_box = collision_mask.bounding_box;

    Collision_result result;
    result.collided = false;
    result.stuck = false;
    result.subject = subject.entity;
    result.object = nullptr;

    if (!collision_mask.pattern) {
        return result;
    }

    const auto& pattern = *collision_mask.pattern;

    auto old_cos = int(std::cos(old_rotation.radians()) * 65536.0 + 0.5);
    auto old_sin = int(std::sin(old_rotation.radians()) * 65536.0 + 0.5);

    auto new_cos = int(std::cos(new_rotation.radians()) * 65536.0 + 0.5);
    auto new_sin = int(std::sin(new_rotation.radians()) * 65536.0 + 0.5);

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

    for (auto y = bounding_box.top, bottom = bounding_box.bottom(); y != bottom; ++y) {
        for (auto x = bounding_box.left, right = bounding_box.right(); x != right; ++x) {
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


                    auto& position = result.subject_position;
                    auto& rotation = result.subject_rotation;         
                    
                    position.x = old_position.x + std::round(offset.x * collision_point.time_point) + adjustment.x;
                    position.y = old_position.y + std::round(offset.y * collision_point.time_point) + adjustment.y;

                    if (offset.x < 0 && position.x > old_position.x) position.x = old_position.x;
                    if (offset.x > 0 && position.x < old_position.x) position.x = old_position.x;

                    if (offset.y < 0 && position.y > old_position.y) position.y = old_position.y;
                    if (offset.y > 0 && position.y < old_position.y) position.y = old_position.y;

                    
                    rotation = old_rotation + Rotation<double>::radians((new_rotation - old_rotation).radians() * collision_point.time_point);

                    // Put the entity in the middle of the pixel, to keep things balanced.
                    position.x += 0.5;
                    position.y += 0.5;
                }
            }
        }
    }

    // Test if car in wall, this is ass
    if (result.collided) 
    {
        new_cos = int(std::cos(new_rotation.radians()) * 65536.0 + 0.5);
        new_sin = int(std::sin(new_rotation.radians()) * 65536.0 + 0.5);

        auto entity_in_wall = [&]()
        {
            for (auto y = bounding_box.top, bottom = bounding_box.bottom(); y != bottom; ++y) {
                for (auto x = bounding_box.left, right = bounding_box.right(); x != right; ++x) {
                    if (pattern(x, y) == 0) continue;

                    Vector2i point{ x, y };
                    point -= center;

                    Vector2i transformed{ (old_cos * point.x - old_sin * point.y + 0x8000) >> 16,
                        (old_sin * point.x + old_cos * point.y + 0x8000) >> 16 };

                    transformed += Vector2i(result.subject_position);

                    if (wall_test(transformed)) return true;
                }
            }

            return false;
        };

        if (entity_in_wall()) {
            result.subject_position = old_position;
            result.subject_rotation = old_rotation;
            result.normal.x = -offset.x;
            result.normal.y = -offset.y;
        }
    }


    return result;
}

ts::world::Collision_result ts::world::detect_collision(const Entity_state& subject, const Entity_state& object)
{
    const auto& entity = *subject.entity;

    Vector2i new_position = subject.position;
    const auto& new_rotation = subject.rotation;

    Vector2i object_new_position = object.position;
    const auto& object_rotation = object.rotation;

    auto old_rotation = entity.rotation();
    Vector2i old_position = entity.position();

    Vector2i object_old_position = object.entity->position();

    const auto& collision_mask = entity.collision_mask();
    const auto& object_mask = object.entity->collision_mask();

    auto z_level = entity.z_level();
    

    Collision_result result;
    result.collided = false;
    result.stuck = false;
    result.subject = subject.entity;
    result.object = object.entity;


    if (!collision_mask.pattern || !object_mask.pattern || z_level != object.entity->z_level()) {
        return result;
    }

    const auto& bounding_box = collision_mask.bounding_box;
    const auto& object_bounding_box = object_mask.bounding_box;    

    const auto& pattern = *collision_mask.pattern;
    const auto& object_pattern = *object_mask.pattern;

    auto old_cos = int(std::cos(old_rotation.radians()) * 65536.0 + 0.5);
    auto old_sin = int(std::sin(old_rotation.radians()) * 65536.0 + 0.5);

    auto new_cos = int(std::cos(new_rotation.radians()) * 65536.0 + 0.5);
    auto new_sin = int(std::sin(new_rotation.radians()) * 65536.0 + 0.5);

    auto object_cos = int(std::cos(object_rotation.radians()) * 65536.0 + 0.5);
    auto object_sin = int(std::sin(object_rotation.radians()) * 65536.0 + 0.5);

    auto offset = new_position - old_position;

    Vector2i center{ bounding_box.left + (bounding_box.width >> 1),
        bounding_box.top + (bounding_box.height >> 1) };

    Vector2i object_center{ object_bounding_box.left + (object_bounding_box.width >> 1),
        object_bounding_box.top + (object_bounding_box.height >> 1)
    };

    // For every point in the subject's collision mask
        // Draw a line from the subject's old position to its new position
        // For every pixel in this line,
           // Calculate the corresponding coordinate in the subject's collision mask
           // at that point in time, and test if it intersects

    for (auto y = bounding_box.top, bottom = bounding_box.bottom(); y != bottom; ++y) {
        for (auto x = bounding_box.left, right = bounding_box.right(); x != right; ++x) {
            if (pattern(x, y) == 0) continue;

            Vector2i point{ x, y };
            point -= center;

            Vector2i old_transformed{ (old_cos * point.x - old_sin * point.y + 0x8000) >> 16,
                (old_sin * point.x + old_cos * point.y + 0x8000) >> 16 };

            Vector2i new_transformed{ (new_cos * point.x - new_sin * point.y + 0x8000) >> 16,
                (new_sin * point.x + new_cos * point.y + 0x8000) >> 16 };

            old_transformed += old_position;
            new_transformed += new_position;

            Vector2i transformed_offset = new_transformed - old_transformed;

            auto test_collision = [&](const Vector2i& point)
            {
                auto frame_offset = point - old_transformed;
                int time_point = 0xFFFF;
                if (transformed_offset.x != 0 && transformed_offset.y != 0) {
                    auto x = (frame_offset.x << 16) / transformed_offset.x;
                    auto y = (frame_offset.y << 16) / transformed_offset.y;
                    time_point = (x + y) >> 1;
                }

                else if (transformed_offset.x != 0) {
                    time_point = (frame_offset.x << 16) / transformed_offset.x;
                }

                else if (transformed_offset.y != 0) {
                    time_point = (frame_offset.y << 16) / transformed_offset.y;
                }

                // Get object position at time point
                Vector2i position = { object_old_position.x + (((object_new_position.x - object_old_position.x) * time_point) >> 16),
                    object_old_position.y + (((object_new_position.y - object_old_position.y) * time_point) >> 16) };

                auto local_position = point - position;

                local_position = { (object_cos * local_position.x - object_sin * local_position.y + 0x8000) >> 16,
                                   (object_cos * local_position.y - object_sin * local_position.y + 0x8000) >> 16 };

                local_position += object_center;

                if (local_position.x < 0 || local_position.x >= object_bounding_box.right() ||
                    local_position.y < 0 || local_position.y >= object_bounding_box.bottom()) return false;

                return object_pattern(local_position.x, local_position.y) != 0;
            };


            auto collision_point = find_wall_between_points(old_transformed, new_transformed, test_collision);
            if (collision_point.collided) {
                result.collided = true;
            }
        }
    }

    return result;
}
