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
            Vector2<double> global_point;
            double time_point;
            double valid_time_point;
        };


        template <typename WallTest>
        Vector2i get_edge_normal(const resources::Pattern& pattern, WallTest is_wall, Vector2i point, Vector2i origin);

        Vector2<double> interpolate_position(Vector2<double> old_pos, Vector2<double> new_pos, double time_point)
        {
            return{ old_pos.x + ((new_pos.x - old_pos.x) * time_point),
                old_pos.y + ((new_pos.y - old_pos.y) * time_point) };
        }

        Vector2<double> transform_point(Vector2<double> point, double sin, double cos)
        {
            return{ cos * point.x - sin * point.y, sin * point.x + cos * point.y };
        };

        template <typename CollisionTest>
        ts::world::Collision_point test_trajectory(Vector2<double> start, Vector2<double> end, CollisionTest test_func);
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

    auto make_normal = [offset_from_id, offset](int a, int b)
    {
        auto normal = offset_from_id(a) - offset_from_id(b);
        std::swap(normal.x, normal.y);

        bool steep = std::abs(offset.y) > std::abs(offset.x);

        if (steep) {
            if ((offset.y < 0 && normal.y < 0) || (offset.y > 0 && normal.y > 0)) normal.y = -normal.y;
            else normal.x = -normal.x;
        }

        else {
            if ((offset.x < 0 && normal.x < 0) || (offset.x > 0 && normal.x > 0)) normal.x = -normal.x;
            else normal.y = -normal.y;
        }
        

        return normal;
    };

    int testing_order[8] = { 0, 3, 6, 7, 8, 5, 2, 1 };

    int steep = std::abs(offset.y) > std::abs(offset.x);

    if (offset.x == 0) {
        if (offset.y < 0) std::rotate(std::begin(testing_order), std::begin(testing_order) + 4, std::end(testing_order));
    }

    else if (offset.y == 0) {
        if (offset.x < 0) std::rotate(std::begin(testing_order), std::end(testing_order) - 2, std::end(testing_order));
        else std::rotate(std::begin(testing_order), std::begin(testing_order) + 2, std::end(testing_order));
    }

    else if (offset.y < 0) {
        if (offset.x < 0) std::rotate(std::begin(testing_order), std::end(testing_order) - 3 - steep, std::end(testing_order));
        else std::rotate(std::begin(testing_order), std::begin(testing_order) + 2 + steep, std::end(testing_order));
    }

    else if (offset.y > 0) {
        if (offset.x > 0) std::rotate(std::begin(testing_order), std::begin(testing_order) + 1 - steep, std::end(testing_order));
        else std::rotate(std::begin(testing_order), std::end(testing_order) - 2 + steep, std::end(testing_order));
    }

    auto predicate = [&surrounding_points](int point_id)
    {
        return surrounding_points[point_id];
    };

    auto it_a = std::find_if(std::begin(testing_order), std::end(testing_order), predicate);
    auto it_b = std::find_if(std::rbegin(testing_order), std::rend(testing_order), predicate);

    if (it_a == std::end(testing_order) || it_b == std::rend(testing_order) || *it_a == *it_b) {
        if (steep) {
            return { 0, offset.y < 0 ? 1 : -1 };
        }

        else if (std::abs(offset.x) > std::abs(offset.y)) {
            return { offset.x < 0 ? 1 : -1, 0 };
        }

        return { offset.x < 0 ? 1 : -1, offset.y < 0 ? 1 : -1 };
    }

    return make_normal(*it_a, *it_b);
}

template <typename CollisionTest>
ts::world::Collision_point ts::world::test_trajectory(Vector2<double> start, Vector2<double> end, CollisionTest test_func)
{
    Collision_point result;
    result.collided = false;

    auto offset = end - start;
    auto frame_count = std::max(std::abs(int(offset.x)), std::abs(int(offset.y))) + 1;

    double time_point = 0.0;
    double valid_time_point = -1.0;
    auto time_step = 1.0 / frame_count;

    for (auto frame = 0; frame <= frame_count; ++frame)
    {
        auto point = interpolate_position(start, end, time_point);

        result = test_func(point, time_point);
        if (result.collided) {
            result.time_point = time_point;
            result.valid_time_point = valid_time_point;
            return result;
        }

        valid_time_point = time_point; 
        time_point += time_step;
    }

    return result;
}

ts::world::Collision_result ts::world::detect_collision(const Entity_state& subject, const resources::Pattern& scenery, const resources::Terrain_library& terrain_lib)
{
    const auto& entity = *subject.entity;
    const auto new_position = subject.position;
    const auto new_rotation = subject.rotation;

    const auto old_rotation = entity.rotation();
    const auto old_position = entity.position();

    auto z_level = entity.z_level();

    const auto& collision_mask = entity.collision_mask();
    const auto& bounding_box = collision_mask.bounding_box;

    Collision_result result;
    result.collided = false;
    result.stuck = false;
    result.rotate = false;
    result.bounciness = 0.0;
    result.subject_position = old_position;
    result.subject_rotation = subject.entity->rotation();
    result.subject = subject.entity;
    result.object = nullptr;

    if (!collision_mask.pattern) {
        return result;
    }

    const auto& pattern = *collision_mask.pattern;

    auto old_cos = std::cos(old_rotation.radians());
    auto old_sin = std::sin(old_rotation.radians());

    auto new_cos = std::cos(new_rotation.radians());
    auto new_sin = std::sin(new_rotation.radians());

    auto offset = new_position - old_position;

    Vector2i center{ bounding_box.width >> 1, bounding_box.height >> 1 };

    Vector2i scenery_size = scenery.size();

    
    auto is_wall = [&](Vector2i point)
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

            Vector2i global_point = transform_point(point, old_sin, old_cos) + old_position;
            if (is_wall(global_point)) {
                result.stuck = true;
                result.collided = true;
                return result;
            }
        }
    }

    auto collision_test = [&](Vector2d position, double time_point)
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

                Vector2i global_point = transform_point(point, new_sin, new_cos) + position;

                if (is_wall(global_point)) {
                    return collision(global_point);
                }
            }
        }

        return collision_point;
    };

    auto collision_point = test_trajectory(old_position, new_position, collision_test);
    if (collision_point.collided)
    {
        result.collided = true;

        if (collision_point.valid_time_point >= 0.0) {
            result.subject_position = interpolate_position(old_position, new_position, collision_point.valid_time_point);
            result.subject_rotation = subject.rotation;
            result.rotate = true;
            result.time_point = collision_point.valid_time_point;
        }

        else {
            result.time_point = 0.0;
        }

        // Get the normal
        auto collision_position = interpolate_position(old_position, new_position, collision_point.time_point);
        auto local_point = transform_point(old_position - collision_point.global_point, -new_sin, new_cos);
        auto origin = transform_point(local_point, old_sin, old_cos) + collision_position;

        auto normal = get_edge_normal(scenery, is_wall, collision_point.global_point, origin);

        result.normal = normalize<double>(normal);

        Vector2i global_point = collision_point.global_point;        
        auto terrain_id = scenery(global_point.x, global_point.y);
        terrain_id = terrain_lib.sub_terrain(terrain_id, z_level);
        result.bounciness = terrain_lib.terrain_by_id(terrain_id).wall_definition.elasticity;
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

    auto subject_old_position = subject.entity->position();
    auto subject_new_position = subject.position;

    auto object_old_position = object.entity->position();
    auto object_new_position = object.position;

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
        double old_cos;
        double old_sin;

        double new_cos;
        double new_sin;
    };

    auto make_trig = [](const Entity_state& state)
    {
        Entity_trig trig;
        trig.old_cos = std::cos(state.entity->rotation().radians());
        trig.old_sin = std::sin(state.entity->rotation().radians());
        trig.new_cos = std::cos(state.rotation.radians());
        trig.new_sin = std::sin(state.rotation.radians());

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
            : is_wall_(is_wall)
        {}

        Collision_point operator()(const Vector2d& position, double time_point)
        {
            Collision_point cp;
            cp.time_point = time_point;
            cp.collided = is_wall_(position);
            return cp;
        }

    private:
        Wall_test is_wall_;
    };

    const auto& subject_mask = subject.entity->collision_mask();
    const auto& object_mask = object.entity->collision_mask();

    Vector2i subject_center{ subject_mask.bounding_box.width >> 1, subject_mask.bounding_box.height >> 1 };
    Vector2i object_center{ object_mask.bounding_box.width >> 1, object_mask.bounding_box.height >> 1 };

    Entity_trig subject_trig = make_trig(subject);
    Entity_trig object_trig = make_trig(object);

    auto bounding_box_test = [&]()
    {
        auto make_bb = [](Vector2i old_position, Vector2i new_position, const Entity_trig& trig, Vector2i center)
        {

            auto old_width = int(std::abs(center.x * trig.old_cos) + std::abs(center.y * trig.old_sin) + 1.0);
            auto new_width = int(std::abs(center.x * trig.new_cos) + std::abs(center.y * trig.new_sin) + 1.0);

            auto old_height = int(std::abs(center.y * trig.old_cos) + std::abs(center.x * trig.old_sin) + 1.0);
            auto new_height = int(std::abs(center.y * trig.old_cos) + std::abs(center.x * trig.old_sin) + 1.0);

            old_width >>= 1;
            new_width >>= 1;
                 
            old_height >>= 1;
            new_height >>= 1;

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
        return result;
    }

    const auto& subject_pattern = *subject_mask.pattern;
    const auto& object_pattern = *object_mask.pattern;

    auto subject_offset = subject_new_position - subject_old_position;
    auto object_offset = object_new_position - object_old_position;

    Wall_test object_wall_test(object_pattern, object_mask.bounding_box);
    Wall_test subject_wall_test(subject_pattern, subject_mask.bounding_box);

    Collision_test object_collision_test(object_wall_test);

    Collision_point collision_point;
    collision_point.collided = false;

    for (auto y = subject_mask.bounding_box.top, bottom = subject_mask.bounding_box.bottom(); y != bottom; ++y) {
        for (auto x = subject_mask.bounding_box.left, right = subject_mask.bounding_box.right(); x != right; ++x) {
            if (subject_pattern(x, y) == 0) continue;

            Vector2i point{ x, y };
            point -= subject_center;

            auto pre_rotation_global = transform_point(point, subject_trig.old_sin, subject_trig.old_cos) + subject_old_position;
            auto pre_rotation_local = transform_point(pre_rotation_global - object_old_position,
                                                            -object_trig.old_sin, object_trig.old_cos);

            pre_rotation_local += object_center;

            

            if (object_wall_test(pre_rotation_local)) {
                result.stuck = true;
                result.collided = true;
                result.time_point = 0.0;
                return result;
            }

            auto old_global = transform_point(point, subject_trig.new_sin, subject_trig.new_cos) + subject_old_position;
            auto old_local = transform_point(old_global - object_old_position,
                                             -object_trig.new_sin, object_trig.new_cos);

            old_local += object_center;

            auto new_global = transform_point(point, subject_trig.new_sin, subject_trig.new_cos) + subject_new_position;
            auto new_local = transform_point(new_global - object_new_position,
                                                 -object_trig.new_sin, object_trig.new_cos);

            new_local += object_center;

            auto cp = test_trajectory(old_local, new_local, object_collision_test);
            if (cp.collided && (!collision_point.collided || cp.valid_time_point < collision_point.valid_time_point))
            {
                collision_point = cp;
                collision_point.global_point = interpolate_position(old_global, new_global, cp.time_point);
            }
        }
    }

    if (collision_point.collided)
    {
        auto time_point = collision_point.valid_time_point;

        if (time_point >= 0.0) {            
            if (time_point != 0.0) {
                result.subject_position = interpolate_position(subject_old_position, subject_new_position, time_point);
                result.object_position = interpolate_position(object_old_position, object_new_position, time_point);
            } 

            result.subject_rotation = subject.rotation;
            result.object_rotation = object.rotation;

            result.time_point = time_point;
            result.rotate = true;
        }

        else {
            result.time_point = 0.0;
        }

        result.collided = true;
    }

    return result;
}
