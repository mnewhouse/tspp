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

#include "core/utility.hpp"

#include <algorithm>

namespace ts
{
    namespace world
    {
        template <typename WallChecker>
        Collision_point find_wall_between_points(Vector2i first_point, Vector2i second_point, WallChecker is_wall);

        template <typename WallTest>
        Vector2i get_edge_normal(const resources::Pattern& pattern, WallTest is_wall, Vector2i point, Vector2i origin);

        Vector2i adjust_position(Vector2i position, Vector2i normal, Vector2i offset);

        Vector2i interpolate_position(Vector2i old_pos, Vector2i new_pos, int time_point)
        {
            return{ old_pos.x + (((new_pos.x - old_pos.x) * time_point) >> 16),
                old_pos.y + (((new_pos.y - old_pos.y) * time_point) >> 16) };
        }
        
        Vector2i transform_point(int isin, int icos, Vector2i point)
        {
            return{ (icos * point.x - isin * point.y + 0x8000) >> 16, (isin * point.x + icos * point.y + 0x8000) >> 16 };
        };

        Vector2i inverse_transform_point(int isin, int icos, Vector2i point)
        {
            return{ (icos * point.x + isin * point.y + 0x8000) >> 16, (icos * point.y - isin * point.x + 0x8000) >> 16 };
        };
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
ts::Vector2i ts::world::get_edge_normal(const resources::Pattern& pattern, WallTest is_wall, Vector2i point, Vector2i origin)
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
        return Vector2i{ 0, offset.y > 0 ? 1 : -1 };
    }
        
    return Vector2i{ offset.x > 0 ? 1 : -1, 0 };
}

ts::Vector2i ts::world::adjust_position(Vector2i position, Vector2i old_position, Vector2i new_position)
{
    auto offset = new_position - old_position;

    auto steep = std::abs(offset.y) > std::abs(offset.x);

    if (steep) {
        if (offset.y < 0) position.y += 1;
        else if (offset.y > 0) position.y -= 1;

        position.x = old_position.x + ((old_position.y - position.y) * offset.x / offset.y);
    }

    else {
        if (offset.x < 0) position.x += 1;
        else if (offset.x > 0) position.x -= 1;

        if (offset.x != 0) {
            position.y = old_position.y + ((old_position.x - position.x) * offset.y / offset.x);
        }
    }

    return position;
};


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
    result.subject_position = subject.entity->position();
    result.subject_rotation = subject.entity->rotation();
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

    if (offset.x == 0 && offset.y == 0 && old_cos == new_cos && old_sin == new_sin) {
        return result;
    }

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

            Vector2i old_transformed = transform_point(old_sin, old_cos, point) + old_position;
            Vector2i new_transformed = transform_point(new_sin, new_cos, point) + new_position;

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
                }
            }
        }
    }

    if (result.collided)
    {
        result.normal = normalize(result.normal);

        auto position = interpolate_position(old_position, new_position, int(result.time_point * 65536.0));

        position = adjust_position(position, old_position, new_position);

        position.x = clamp(position.x, old_position.x, new_position.x);
        position.y = clamp(position.y, old_position.y, new_position.y);

        result.subject_rotation = subject.entity->rotation();

        // Put the entity in the middle of the pixel, to keep things balanced.
        result.subject_position.x = position.x + 0.5;
        result.subject_position.y = position.y + 0.5;

        // Test if car in wall, this is ass
        auto entity_in_wall = [&]()
        {
            for (auto y = bounding_box.top, bottom = bounding_box.bottom(); y != bottom; ++y) {
                for (auto x = bounding_box.left, right = bounding_box.right(); x != right; ++x) {
                    if (pattern(x, y) == 0) continue;

                    Vector2i point{ x, y };
                    point -= center;

                    auto transformed = transform_point(old_sin, old_cos, point) + position;

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

ts::world::Collision_result ts::world::detect_collision(Entity_state subject, Entity_state object)
{
    {
        auto position_diff = subject.entity->position() - object.entity->position();

        auto subj_dot = dot_product(position_diff, normalize(subject.entity->position() - subject.position));
        auto obj_dot = dot_product(-position_diff, normalize(object.entity->position() - object.position));

        if (obj_dot > subj_dot) {
            // Roles are reversed - treat the object as the subject.
            std::swap(subject, object);
        }
    }

    Collision_result result;
    result.collided = false;
    result.stuck = false;
    result.subject = subject.entity;
    result.object = object.entity;

    const auto& subject_mask = subject.entity->collision_mask();
    const auto& object_mask = object.entity->collision_mask();

    if (!subject_mask.pattern || !object_mask.pattern || subject.entity->z_level() != object.entity->z_level()) 
    {
        // These entities can't possibly collide with each other.
        return result;
    }

    // TODO: Bounding box check.

    struct Entity_trig
    {
        int old_cos;
        int old_sin;

        int new_cos;
        int new_sin;
    };

    auto make_trig = [](const Entity_state& state)
    {
        Entity_trig trig;
        trig.old_cos = int(std::cos(state.entity->rotation().radians()) * 65536.0 + 0.5);
        trig.old_sin = int(std::sin(state.entity->rotation().radians()) * 65536.0 + 0.5);
        trig.new_cos = int(std::cos(state.rotation.radians()) * 65536.0 + 0.5);
        trig.new_sin = int(std::sin(state.rotation.radians()) * 65536.0 + 0.5);

        return trig;
    };

    Entity_trig subject_trig = make_trig(subject);
    Entity_trig object_trig = make_trig(object);

    auto make_center = [](const Rect<int>& bounding_box) -> Vector2i
    {
        return { bounding_box.left + (bounding_box.width >> 1), bounding_box.top + (bounding_box.height >> 1) };
    };

    auto subject_center = make_center(subject_mask.bounding_box);
    auto object_center = make_center(object_mask.bounding_box);

    const auto& subject_pattern = *subject_mask.pattern;
    const auto& object_pattern = *object_mask.pattern;

    Vector2i subject_old_position = subject.entity->position();
    Vector2i subject_new_position = subject.position;

    Vector2i object_old_position = object.entity->position();
    Vector2i object_new_position = object.position;

    auto subject_offset = subject_new_position - subject_old_position;
    auto object_offset = object_new_position - object_old_position;

    auto get_time_point = [](Vector2i offset, Vector2i total_offset)
    {
        if (total_offset.x != 0 && total_offset.y != 0) {
            auto x = (offset.x << 16) / total_offset.x;
            auto y = (offset.y << 16) / total_offset.y;
            return (x + y) >> 1;
        }

        else if (offset.x != 0) {
            return (offset.x << 16) / total_offset.x;
        }

        else if (offset.y != 0) {
            return (offset.y << 16) / total_offset.y;
        }

        return 0;
    };

    // For every point in the subject's collision mask (or its outline, TODO)
        // Draw a line from the subject's old position to its new position
        // For every pixel in this line,
           // Calculate the corresponding coordinate in the object's collision mask
           // at that point in time, and test if it intersects

    for (auto y = subject_mask.bounding_box.top, bottom = subject_mask.bounding_box.bottom(); y != bottom; ++y)
    {
        for (auto x = subject_mask.bounding_box.left, right = subject_mask.bounding_box.right(); x != right; ++x)
        {
            if (subject_pattern(x, y) == 0) continue;

            auto point = Vector2i{ x, y } -subject_center;

            auto transformed_point = transform_point(subject_trig.new_sin, subject_trig.new_cos, point);

            auto old_transformed = transformed_point + subject_old_position;
            auto new_transformed = transformed_point + subject_new_position;


            auto collision_test = [&](Vector2i point)
            {
                auto offset = point - old_transformed;

                auto time_point = get_time_point(offset, subject_offset);

                auto position = interpolate_position(object_old_position, object_new_position, time_point);

                auto local_position = inverse_transform_point(object_trig.new_sin, object_trig.new_cos, point - position) + object_center;
                if (!contains(object_mask.bounding_box, local_position)) return false;

                return object_pattern(local_position.x, local_position.y) != 0;
            };

            auto collision_point = find_wall_between_points(old_transformed, new_transformed, collision_test);

            if (collision_point.collided && (!result.collided || collision_point.time_point <= result.time_point))
            {
                Vector2i subject_position = collision_point.global_point - transformed_point;

                auto time_int = int(result.time_point * 65536.0);
                Vector2i object_position = interpolate_position(object_old_position, object_new_position, time_int);

                auto is_wall = [&](Vector2i point)
                {
                    if (!contains(object_mask.bounding_box, point)) return false;

                    return object_pattern(point.x, point.y) != 0;
                };


                auto old_point = transform_point(subject_trig.old_sin, subject_trig.old_cos, point) + subject_old_position;

                auto local_point = inverse_transform_point(object_trig.new_sin, object_trig.new_cos, collision_point.global_point - object_position) + object_center;
                auto origin = inverse_transform_point(object_trig.new_sin, object_trig.new_cos, old_point - object_position);



                auto edge_normal = get_edge_normal(object_pattern, is_wall, local_point, origin + object_center);

                edge_normal = transform_point(object_trig.new_sin, object_trig.new_cos, edge_normal);

                auto normal = normalize<double>(edge_normal);

                if (!result.collided || collision_point.time_point < result.time_point ||
                    dot_product(normal, -subject_offset) > dot_product(result.normal, -subject_offset))
                {
                    result.normal = normal;

                    subject_position = adjust_position(subject_position, subject_old_position, subject_new_position);
                    object_position = adjust_position(object_position, object_old_position, object_new_position);

                    subject_position.x = clamp(subject_position.x, subject_old_position.x, subject_new_position.x);
                    subject_position.y = clamp(subject_position.y, subject_old_position.y, subject_new_position.y);

                    object_position.x = clamp(object_position.x, object_old_position.x, object_new_position.x);
                    object_position.y = clamp(object_position.y, object_old_position.y, object_new_position.y);

                    result.subject_position = subject_position;
                    result.object_position = object_position;

                    result.subject_position += {0.5, 0.5};
                    result.object_position += {0.5, 0.5};

                    result.subject_rotation = subject.entity->rotation();
                    result.object_rotation = object.entity->rotation();
                }

                result.time_point = collision_point.time_point;
                result.collided = true;
            }
        }
    }

    return result;
}
