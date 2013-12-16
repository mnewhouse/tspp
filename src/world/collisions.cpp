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
        struct Collision_point
        {
            bool collided;
            Vector2i global_point;
            int time_point;
            int valid_time_point;
        };


        template <typename WallTest>
        Vector2i get_edge_normal(const resources::Pattern& pattern, WallTest is_wall, Vector2i point, Vector2i origin);

        Vector2i adjust_position(Vector2i position, Vector2i normal, Vector2i offset);

        Vector2i interpolate_position(Vector2i old_pos, Vector2i new_pos, int time_point)
        {
            return{ old_pos.x + (((new_pos.x - old_pos.x) * time_point + 0x8000) >> 16),
                old_pos.y + (((new_pos.y - old_pos.y) * time_point + 0x8000) >> 16) };
        }

        Vector2i transform_point(int isin, int icos, Vector2i point)
        {
            return{ (icos * point.x - isin * point.y + 0x8000) >> 16, (isin * point.x + icos * point.y + 0x8000) >> 16 };
        };

        template <typename CollisionTest>
        ts::world::Collision_point test_trajectory(Vector2i start, Vector2i end, CollisionTest test_func);
    }
}

template <typename WallTest>
ts::Vector2i ts::world::get_edge_normal(const resources::Pattern& pattern, WallTest is_wall, Vector2i point, Vector2i origin)
{
    auto offset = point - origin;

    bool surrounding_points[] = {
        is_wall({ point.x - 1, point.y - 1 }),
        is_wall({ point.x, point.y - 1 }),
        is_wall({ point.x + 1, point.y - 1 }),

        is_wall({ point.x - 1, point.y }),
        is_wall({ point.x, point.y }),
        is_wall({ point.x + 1, point.y }),

        is_wall({ point.x - 1, point.y + 1 }),
        is_wall({ point.x, point.y + 1 }),
        is_wall({ point.x + 1, point.y + 1 })
    };

    auto offset_from_id = [](int id)
    {
        Vector2i offset{};

        switch (id) {
        case 0: case 3: case 6: --offset.x; break;
        case 2: case 5: case 8: ++offset.x; break;
        default: break;
        }

        switch (id) {
        case 0: case 1: case 2: --offset.y; break;
        case 6: case 7: case 8: ++offset.y; break;
        default: break;
        }

        return offset;
    };

    auto make_normal = [offset_from_id, point, offset](int a, int b)
    {
        auto difference = offset_from_id(a) - offset_from_id(b);
        std::swap(difference.x, difference.y);

        difference.x = std::abs(difference.x);
        difference.y = std::abs(difference.y);
        
        if (offset.x > 0) difference.x = -difference.x;
        if (offset.y > 0) difference.y = -difference.y;

        return difference;
    };

    auto find_surrounding_points = [&surrounding_points](int a, int b, int increment)
    {
        for (int i = 0; i != 2 && !surrounding_points[a]; ++i, a += increment);
        for (int i = 0; i != 2 && !surrounding_points[b]; ++i, b += increment);

        return std::make_pair(a, b);
    };

    if (offset.x < 0) {
        auto pair = find_surrounding_points(2, 8, -1);

        if (surrounding_points[pair.first] && surrounding_points[pair.second]) {
            return make_normal(pair.first, pair.second);
        }
    }

    else if (offset.x > 0) {
        auto pair = find_surrounding_points(0, 6, +1);

        if (surrounding_points[pair.first] && surrounding_points[pair.second]) {
            return make_normal(pair.first, pair.second);
        }
    }

    if (offset.y < 0) {
        auto pair = find_surrounding_points(6, 8, -3);

        if (surrounding_points[pair.first] && surrounding_points[pair.second]) {
            return make_normal(pair.first, pair.second);
        }
    }

    else if (offset.y > 0) {
        auto pair = find_surrounding_points(0, 2, +3);

        if (surrounding_points[pair.first] && surrounding_points[pair.second]) {
            return make_normal(pair.first, pair.second);
        }
    }

    // Now attempt to find two-pixel normal    
    if (offset.x < 0) {
        auto pair = find_surrounding_points(2, 5, -1);

        if (surrounding_points[pair.first] && surrounding_points[pair.second]) {
            return make_normal(pair.first, pair.second);
        }

        pair = find_surrounding_points(5, 8, -1);
        if (surrounding_points[pair.first] && surrounding_points[pair.second]) {
            return make_normal(pair.first, pair.second);
        }
    }

    else if (offset.x > 0) {
        auto pair = find_surrounding_points(0, 3, +1);

        if (surrounding_points[pair.first] && surrounding_points[pair.second]) {
            return make_normal(pair.first, pair.second);
        }

        pair = find_surrounding_points(3, 6, +1);
        if (surrounding_points[pair.first] && surrounding_points[pair.second]) {
            return make_normal(pair.first, pair.second);
        }
    }

    if (offset.y < 0) {
        auto pair = find_surrounding_points(6, 7, -3);

        if (surrounding_points[pair.first] && surrounding_points[pair.second]) {
            return make_normal(pair.first, pair.second);
        }

        pair = find_surrounding_points(7, 8, -3);

        if (surrounding_points[pair.first] && surrounding_points[pair.second]) {
            return make_normal(pair.first, pair.second);
        }
    }

    else if (offset.y > 0) {
        auto pair = find_surrounding_points(0, 1, +3);

        if (surrounding_points[pair.first] && surrounding_points[pair.second]) {
            return make_normal(pair.first, pair.second);
        }

        pair = find_surrounding_points(1, 2, +3);

        if (surrounding_points[pair.first] && surrounding_points[pair.second]) {
            return make_normal(pair.first, pair.second);
        }
    }

    if (std::abs(offset.x) > std::abs(offset.y)) {
        return { offset.x < 0 ? 1 : -1, 0 };
    }

    if (std::abs(offset.y > std::abs(offset.x))) {
        return { 0, offset.y < 0 ? 1 : -1 };
    }

    return { offset.x < 0 ? 1 : -1, offset.y < 0 ? 1 : -1 };
}

template <typename CollisionTest>
ts::world::Collision_point ts::world::test_trajectory(Vector2i start, Vector2i end, CollisionTest test_func)
{
    Collision_point result;
    result.collided = false;
    result.time_point = 0;   

    auto frame_count = std::max(std::abs(start.x - end.x), std::abs(start.y - end.y)) + 1;    
    int valid_time_point = -1;

    for (auto frame = 0; frame <= frame_count; ++frame)
    {
        result.time_point = (frame << 16) / frame_count; 
        auto point = interpolate_position(start, end, result.time_point);

        result = test_func(point, result.time_point);
        if (result.collided) {
            result.valid_time_point = valid_time_point;
            return result;
        }

        valid_time_point = result.time_point; 
    }

    return result;
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
    result.rotate = false;
    result.subject_position = old_position;
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

    Vector2i center{ bounding_box.left + (bounding_box.width >> 1),
        bounding_box.top + (bounding_box.height >> 1) };

    Vector2i scenery_size = scenery.size();

    
    auto is_wall = [&](Vector2i point)
    {
        if (point.x < 0 || point.x >= scenery_size.x || point.y < 0 || point.y >= scenery_size.y) return false;

        auto terrain_id = scenery(point.x, point.y);
        terrain_id = terrain_lib.sub_terrain(terrain_id, z_level);

        return terrain_lib.terrain_by_id(terrain_id).is_wall;
    };

    auto collision_test = [&](Vector2i position, int time_point)
    {
        Collision_point collision_point;
        collision_point.collided = false;
        collision_point.time_point = time_point;

        auto collision = [&collision_point](Vector2i global_point)
        {
            collision_point.global_point = global_point;
            collision_point.collided = true;
            return collision_point;
        };

        for (auto y = bounding_box.top, bottom = bounding_box.bottom(); y != bottom; ++y) {
            for (auto x = bounding_box.left, right = bounding_box.right(); x != right; ++x) {
                if (pattern(x, y) == 0) continue;

                Vector2i point{ x, y };
                point -= center;

                auto global_point = transform_point(new_sin, new_cos, point) + position;

                if (is_wall(global_point)) {
                    return collision(global_point);
                }
            }
        }

        return collision_point;
    };

    for (auto y = bounding_box.top, bottom = bounding_box.bottom(); y != bottom; ++y) {
        for (auto x = bounding_box.left, right = bounding_box.right(); x != right; ++x) {
            if (pattern(x, y) == 0) continue;

            Vector2i point{ x, y };
            point -= center;

            auto global_point = transform_point(old_sin, old_cos, point) + old_position;
            if (is_wall(global_point)) {
                result.stuck = true;
                result.collided = true;
                return result;
            }
        }
    }

    // Test entity's trajectory, with delta rotation applied
    auto collision_point = test_trajectory(old_position, new_position, collision_test);
    if (collision_point.collided)
    {
        result.collided = true;

        if (collision_point.valid_time_point >= 0) {
            result.subject_position = interpolate_position(old_position, new_position, collision_point.valid_time_point);
            result.subject_rotation = subject.rotation;
            result.rotate = true;
            result.time_point = collision_point.valid_time_point / 65536.0;
        }

        else {
            result.time_point = 0.0;
        }

        // Get the normal
        auto normal = get_edge_normal(scenery, is_wall, collision_point.global_point, collision_point.global_point - offset);

        result.normal = normalize<double>(normal);

        result.subject_position += { 0.5, 0.5 };
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

    Vector2i subject_old_position = subject.entity->position();
    Vector2i subject_new_position = subject.position;

    Vector2i object_old_position = object.entity->position();
    Vector2i object_new_position = object.position;

    Collision_result result;
    result.collided = false;
    result.stuck = false;
    result.rotate = false;
    result.subject = subject.entity;
    result.object = object.entity;

    result.subject_position = subject_old_position;
    result.object_position = object_old_position;
    result.subject_rotation = subject.entity->rotation();
    result.object_rotation = object.entity->rotation();

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

    struct Wall_test
    {
    public:
        Wall_test(const resources::Pattern& pattern, Rect<int> bounding_box)
            : pattern(pattern), bounding_box(bounding_box)
        {}

        bool operator()(Vector2i point)
        {
            if (!contains(bounding_box, point)) return false;

            return pattern(point.x, point.y) != 0;
        }

    private:
        const resources::Pattern& pattern;
        Rect<int> bounding_box;
    };

    struct Collision_test
    {
        Collision_test(Wall_test is_wall)
            : is_wall(is_wall)
        {}

        Collision_point operator()(Vector2i point, int time_point)
        {
            Collision_point collision_point;
            collision_point.time_point = time_point;
            collision_point.global_point = point;
            collision_point.collided = is_wall(point);

            return collision_point;
        };

    private:
        Wall_test is_wall;
    };

    auto test_collision = [&result](const Entity_state& subject, const Entity_state& object, 
                             const Entity_trig& subject_trig, const Entity_trig& object_trig)
    {
        const auto& subject_mask = subject.entity->collision_mask();
        const auto& object_mask = object.entity->collision_mask();

        Vector2i subject_center{ subject_mask.bounding_box.width >> 1, subject_mask.bounding_box.height >> 1 };
        Vector2i object_center{ object_mask.bounding_box.width >> 1, object_mask.bounding_box.height >> 1 };

        Vector2i subject_old_position = subject.entity->position();
        Vector2i subject_new_position = subject.position;

        Vector2i object_old_position = object.entity->position();
        Vector2i object_new_position = object.position;

        auto bounding_box_test = [&]()
        {
            auto make_bb = [](Vector2i old_position, Vector2i new_position, const Entity_trig& trig, Vector2i center)
            {

                auto old_width = std::abs(center.x * trig.old_cos) + std::abs(center.y * trig.old_sin);
                auto new_width = std::abs(center.x * trig.new_cos) + std::abs(center.y * trig.new_sin);

                auto old_height = std::abs(center.y * trig.old_cos) + std::abs(center.x * trig.old_sin);
                auto new_height = std::abs(center.y * trig.old_cos) + std::abs(center.x * trig.old_sin);

                old_width >>= 17;
                new_width >>= 17;

                old_height >>= 17;
                new_height >>= 17;

                std::initializer_list<int> x_coords = {
                    old_position.x - old_width - 1,
                    old_position.x + old_width + 1,

                    new_position.x - new_width - 1,
                    new_position.x + new_width + 1
                };

                std::initializer_list<int> y_coords = {
                    old_position.y - old_height - 1,
                    old_position.y + old_height + 1,

                    new_position.y - new_height - 1,
                    new_position.y + new_height + 1
                };

                auto x = std::minmax(x_coords);
                auto y = std::minmax(y_coords);

                return Rect<int>(x.first, y.first, x.second - x.first, y.second - y.first);
            };

            auto subject_bb = make_bb(subject_old_position, subject_new_position, subject_trig, subject_center);
            auto object_bb = make_bb(object_old_position, object_new_position, object_trig, object_center);

            return intersects(subject_bb, object_bb);
        };

        if (!subject_mask.pattern || !object_mask.pattern || 
            subject.entity->z_level() != object.entity->z_level() ||
            !bounding_box_test())
        {
            // These entities can't possibly collide with each other.
            Collision_point collision_point;
            collision_point.collided = false;
            return collision_point;
        }

        const auto& subject_pattern = *subject_mask.pattern;
        const auto& object_pattern = *object_mask.pattern;

        auto subject_offset = subject_new_position - subject_old_position;
        auto object_offset = object_new_position - object_old_position;

        Wall_test object_wall_test(object_pattern, object_mask.bounding_box);

        Collision_test object_collision_test(object_wall_test);


        // Test pre-rotation, pre-movement state
        for (auto y = subject_mask.bounding_box.top, bottom = subject_mask.bounding_box.bottom(); y != bottom; ++y) {
            for (auto x = subject_mask.bounding_box.left, right = subject_mask.bounding_box.right(); x != right; ++x) {
                if (subject_pattern(x, y) == 0) continue;

                Vector2i point{ x, y };
                point -= subject_center;

                auto global_point = transform_point(subject_trig.old_sin, subject_trig.old_cos, point) + subject_old_position;
                auto local_point = transform_point(-object_trig.old_sin, object_trig.old_cos,
                                                   global_point - object_old_position) + object_center;

                if (object_wall_test(local_point)) {
                    result.stuck = true;

                    Collision_point collision_point;
                    collision_point.collided = true;
                    collision_point.global_point = global_point;
                    collision_point.time_point = 0;
                    collision_point.valid_time_point = -1;
                    return collision_point;
                }
            }
        }

        auto frame_collision_test = [&](Vector2i position, int time_point)
        {
            Collision_point collision_point;
            collision_point.collided = false;
            collision_point.time_point = time_point;
            
            auto object_position = interpolate_position(object_old_position, object_new_position, time_point);

            for (auto y = subject_mask.bounding_box.top, bottom = subject_mask.bounding_box.bottom(); y != bottom; ++y) {
                for (auto x = subject_mask.bounding_box.left, right = subject_mask.bounding_box.right(); x != right; ++x) {
                    if (subject_pattern(x, y) == 0) continue;

                    Vector2i point{ x, y };
                    point -= subject_center;

                    auto global_point = transform_point(subject_trig.new_sin, subject_trig.new_cos, point) + position;
                    auto local_point = transform_point(-object_trig.new_sin, object_trig.new_cos, 
                                                       global_point - object_position) + object_center;

                    if (object_wall_test(local_point)) {
                        collision_point.global_point = global_point;
                        collision_point.collided = true;
                    }
                }
            }

            return collision_point;
        };

        return test_trajectory(subject_old_position, subject_new_position, frame_collision_test);
    };

    Entity_trig subject_trig = make_trig(subject);
    Entity_trig object_trig = make_trig(object);

    auto collision_point = subject.entity < object.entity ?
        test_collision(subject, object, subject_trig, object_trig) :
        test_collision(object, subject, object_trig, subject_trig);

    if (collision_point.collided)
    {
        auto time_point = collision_point.valid_time_point;
        if (time_point >= 0) {            
            if (time_point != 0) {
                result.subject_position = interpolate_position(subject_old_position, subject_new_position, time_point);
                result.object_position = interpolate_position(object_old_position, object_new_position, time_point);
            } 

            result.subject_rotation = subject.rotation;
            result.object_rotation = object.rotation;

            result.time_point = time_point / 65536.0;
            result.rotate = true;
        }

        else {
            result.time_point = 0.0;
        }

        result.subject_position += {0.5, 0.5};
        result.object_position += {0.5, 0.5};

        result.collided = true;
    }

    return result;
}
