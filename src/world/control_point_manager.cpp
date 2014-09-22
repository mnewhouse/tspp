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

#include "stdinc.hpp"
#include "control_point_manager.hpp"
#include "control_point_listener.hpp"
#include "entity.hpp"

namespace ts
{
    bool test_line_intersection(const Vector2i& a, const Vector2i& b, const Vector2i& c, const Vector2i& d, Vector2i& intersection);
    bool is_point_on_line(const Vector2i& a, const Vector2i& b, const Vector2i& point);

    double calculate_time_point(Vector2i old_position, Vector2i new_position, Vector2i intersection);
}

void ts::world::Control_point_manager::add_control_point_listener(Control_point_listener* cp_listener)
{
    control_point_listeners_.push_back(cp_listener);
}

void ts::world::Control_point_manager::remove_control_point_listener(Control_point_listener* cp_listener)
{
    control_point_listeners_.erase(std::remove(control_point_listeners_.begin(), control_point_listeners_.end(), cp_listener),
                                   control_point_listeners_.end());
}


void ts::world::Control_point_manager::set_entity_control_point(const Entity* entity, const Control_point* control_point)
{
    entity_points_[entity] = control_point;
}

void ts::world::Control_point_manager::ignore_control_points_for_entity(const Entity* entity)
{
    entity_points_.erase(entity);
}

const ts::world::Control_point* ts::world::Control_point_manager::create_control_area(Vector2i top_left, Vector2i bottom_right)
{
    auto control_point = std::make_unique<Control_point>();
    control_point->start = top_left;
    control_point->end = bottom_right;
    control_point->type = Control_point::Type::Area;

    dynamic_control_points_.push_back(std::move(control_point));
    return &*dynamic_control_points_.back();
}

const ts::world::Control_point* ts::world::Control_point_manager::create_control_point(Vector2i start, Vector2i end)
{
    auto control_point = std::make_unique<Control_point>();
    control_point->start = start;
    control_point->end = end;
    control_point->type = Control_point::Type::Line;

    dynamic_control_points_.push_back(std::move(control_point));
    return &*dynamic_control_points_.back();
}

void ts::world::Control_point_manager::destroy_control_point(const Control_point* control_point)
{
    auto it = std::find_if(dynamic_control_points_.begin(), dynamic_control_points_.end(), 
                           [control_point](const std::unique_ptr<Control_point>& ptr)
    {
        return ptr.get() == control_point;
    });

    if (it != dynamic_control_points_.end())
    {
        std::swap(*it, dynamic_control_points_.back());
        dynamic_control_points_.pop_back();
    }
}

void ts::world::Control_point_manager::test_control_point_intersection(Entity* entity, Vector2i old_position, Vector2i new_position) const
{
    if (old_position == new_position) return;

    auto it = entity_points_.find(entity);
    if (it == entity_points_.end()) return; // No entry, do nothing

    if (it->second == nullptr)
    {
        return test_all_control_points(entity, old_position, new_position);
    }

    double time_point = 0.0;
    Vector2i intersection;

    while (it != entity_points_.end() && it->second != nullptr)
    {
        if (!test_control_point(entity, it->second, old_position, new_position, intersection, time_point)) break;

        old_position = intersection;

        auto new_it = entity_points_.find(entity);
        if (new_it == it) break;

        it = new_it;
    }
}

bool ts::world::Control_point_manager::test_control_point(Entity* entity, const Control_point* control_point, 
                                                          Vector2i old_position, Vector2i new_position,
                                                          Vector2i& intersection, double& time_point) const
{


    if (control_point->type == Control_point::Type::Line)
    {
        if (test_line_intersection(control_point->start, control_point->end, old_position, new_position, intersection)
            && new_position != intersection)
        {
            auto time_left = 1.0 - time_point;
            time_point = time_point + time_left * calculate_time_point(old_position, new_position, intersection);
            trigger_control_point_hit(entity, control_point, time_point);
            return true;
        }
    }

    else 
    {
        // TODO
    }

    return false;
}

std::int32_t ts::world::Control_point_manager::control_point_id(const Control_point* control_point) const
{
    auto data_ptr = static_control_points_.data();

    if (control_point >= data_ptr && control_point < data_ptr + static_control_points_.size())
    {
        return static_cast<std::int32_t>(control_point - data_ptr);
    }

    return -1;
}

const ts::world::Control_point* ts::world::Control_point_manager::get_control_point_by_id(std::int32_t id) const
{
    if (id >= 0 && id < static_control_points_.size()) return &static_control_points_[id];

    return nullptr;
}

void ts::world::Control_point_manager::on_entity_destroy(Entity* entity)
{
    entity_points_.erase(entity);
}

void ts::world::Control_point_manager::trigger_control_point_hit(Entity* entity, const Control_point* control_point, double time_point) const
{
    
    for (auto listener : control_point_listeners_)
    {
        listener->on_control_point_hit(entity, control_point, time_point);
    }
}

void ts::world::Control_point_manager::test_all_control_points(Entity* entity, Vector2i old_position, Vector2i new_position) const
{
    double time_point = 0.0;
    Vector2i intersection;

    for (const auto& control_point : static_control_points_)
    {
        test_control_point(entity, &control_point, old_position, new_position, intersection, time_point);
    }

    for (const auto& control_point : dynamic_control_points_)
    {
        test_control_point(entity, control_point.get(), old_position, new_position, intersection, time_point);
    }
}

const std::vector<ts::world::Control_point>& ts::world::Control_point_manager::static_control_points() const
{
    return static_control_points_;
}

const std::vector<std::unique_ptr<ts::world::Control_point>>& ts::world::Control_point_manager::dynamic_control_points() const
{
    return dynamic_control_points_;
}

bool ts::is_point_on_line(const Vector2i& a, const Vector2i& b, const Vector2i& point)
{
    auto dummy_point = point;
    if (std::abs(a.x - b.x) > std::abs(a.y - b.y)) ++dummy_point.x;
    else ++dummy_point.y;

    Vector2i intersection;
    return test_line_intersection(a, b, point, dummy_point, intersection);
}

bool ts::test_line_intersection(const Vector2i& a, const Vector2i& b, const Vector2i& c, const Vector2i& d, Vector2i& intersection)
{
    auto denominator = (a.x - b.x) * (c.y - d.y) - (a.y - b.y) * (c.x - d.x);
    if (denominator == 0) return false;

    intersection.x = ((a.x * b.y - a.y * b.x) * (c.x - d.x) - (a.x - b.x) * (c.x * d.y - c.y * d.x)) / denominator;
    intersection.y = ((a.x * b.y - a.y * b.x) * (c.y - d.y) - (a.y - b.y) * (c.x * d.y - c.y * d.x)) / denominator;

    return intersection.x >= std::min(a.x, b.x) && std::max(a.x, b.x) >= intersection.x &&
        intersection.y >= std::min(a.y, b.y) && std::max(a.y, b.y) >= intersection.y &&
        intersection.x >= std::min(c.x, d.x) && std::max(c.x, d.x) >= intersection.x &&
        intersection.y >= std::min(c.y, d.y) && std::max(c.y, d.y) >= intersection.y;
}

double ts::calculate_time_point(Vector2i old_position, Vector2i new_position, Vector2i intersection)
{
    if (std::abs(old_position.x - new_position.x) > std::abs(old_position.y - new_position.y))
    {
        return (intersection.x - old_position.x) / static_cast<double>(new_position.x - old_position.x);
    }

    return (intersection.y - old_position.y) / static_cast<double>(new_position.y - old_position.y);
}