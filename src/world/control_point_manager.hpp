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

#ifndef WORLD_CONTROL_POINT_MANAGER_HPP
#define WORLD_CONTROL_POINT_MANAGER_HPP

#include "world_listener.hpp"

#include "resources/control_point.hpp"
#include "core/vector2.hpp"

#include <vector>
#include <unordered_map>
#include <memory>

namespace ts
{
    namespace world
    {
        class Entity;
        using resources::Control_point;

        struct Control_point_listener;

        class Control_point_manager
            : public World_listener
        {
        public:

            // Takes an input range, representing the list of static control points.
            template <typename InputIt>
            Control_point_manager(InputIt begin, InputIt end);

            const Control_point* create_control_point(Vector2i start, Vector2i end);
            const Control_point* create_control_area(Vector2i top_left, Vector2i bottom_right);
            void destroy_control_point(const Control_point* control_point);

            void add_control_point_listener(Control_point_listener* listener);

            void set_entity_control_point(const Entity* entity, const Control_point* control_point);
            void ignore_control_points_for_entity(const Entity* entity);

            const std::vector<Control_point>& static_control_points() const;
            const std::vector<std::unique_ptr<Control_point>>& dynamic_control_points() const;

            void test_control_point_intersection(Entity* entity, Vector2i old_position) const;

            std::int32_t control_point_id(const Control_point* control_point) const;
            const Control_point* get_control_point_by_id(std::int32_t id) const;

            virtual void on_entity_destroy(Entity* entity) override;

        private:
            struct Control_point_result
            {
                bool hit;
                double time_point;
                Vector2i intersection;

                operator bool() const { return hit; }
            };

            void trigger_control_point_hit(Entity* entity, const Control_point* control_point, double time_point) const;

            void test_all_control_points(Entity* entity, Vector2i old_position, Vector2i new_position) const;
            bool test_control_point(Entity* entity, const Control_point* control_point, 
                                    Vector2i old_position, Vector2i new_position, Vector2i& intersection, double& time_point) const;

            std::unordered_map<const Entity*, const Control_point*> entity_points_;
            std::vector<Control_point> static_control_points_;

            std::vector<std::unique_ptr<Control_point>> dynamic_control_points_;
            std::vector<Control_point_listener*> control_point_listeners_;
        };
    }
}

template <typename InputIt>
ts::world::Control_point_manager::Control_point_manager(InputIt begin, InputIt end)
    : static_control_points_(begin, end)
{
}

#endif