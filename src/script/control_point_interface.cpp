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

#include "control_point_interface.hpp"
#include "world_interface.hpp"
#include "script_definitions.hpp"

#include "world/control_point_manager.hpp"

ts::script::Control_point_interface::Control_point_interface(world::Control_point_manager* cp_manager,
                                                             World_interface* world_interface, Engine* engine)
: cp_manager_(cp_manager),
  world_interface_(world_interface),
  engine_(engine)
{
}

void ts::script::Control_point_interface::addEventListener(asIScriptObject* listener)
{
    callback_list_.store(listener);

    listener->Release();
}

void ts::script::Control_point_interface::removeEventListener(asIScriptObject* listener)
{
    callback_list_.remove(listener);

    listener->Release();
}

ts::script::Control_point_handle* ts::script::Control_point_interface::get_control_point_by_id(std::int32_t id)
{
    auto control_point = cp_manager_->get_control_point_by_id(id);
    return get_control_point_handle(control_point);
}

std::int32_t ts::script::Control_point_interface::get_control_point_id(const world::Control_point* control_point) const
{
    return cp_manager_->control_point_id(control_point);
}

std::uint32_t ts::script::Control_point_interface::get_control_point_count() const
{
    return cp_manager_->static_control_points().size();
}

ts::script::Control_point_handle* ts::script::Control_point_interface::get_control_point_handle(const world::Control_point* control_point)
{
    if (control_point == nullptr) return nullptr;

    auto it = cp_handles_.find(control_point);
    if (it != cp_handles_.end())
    {
        return &it->second;
    }

    Control_point_handle cp_handle;
    cp_handle.control_point = control_point;
    cp_handle.cp_interface = this;
    cp_handle.ref_count = 0;
    cp_handle.dynamic = cp_manager_->control_point_id(control_point) < 0;

    auto result = cp_handles_.insert(std::make_pair(control_point, cp_handle));
    return &result.first->second;
}

ts::script::Control_point_handle* ts::script::Control_point_interface::create_control_point(Vector2i start, Vector2i end)
{
    auto control_point = cp_manager_->create_control_point(start, end);
    auto handle = get_control_point_handle(control_point);
    handle->dynamic = true;

    return handle;
}

void ts::script::Control_point_interface::release_control_point_handle(const Control_point_handle* cp_handle)
{
    auto control_point = cp_handle->control_point;

    if (cp_handle->dynamic) cp_manager_->destroy_control_point(control_point);

    cp_handles_.erase(control_point);
}

void ts::script::Control_point_interface::set_entity_control_point(world::Entity* entity, const world::Control_point* control_point)
{
    cp_manager_->set_entity_control_point(entity, control_point);
}

void ts::script::Control_point_interface::ignore_control_points_for_entity(world::Entity* entity)
{
    cp_manager_->ignore_control_points_for_entity(entity);
}

void ts::script::Control_point_interface::on_control_point_hit(world::Entity* entity, const world::Control_point* control_point, double time_point)
{
    auto entity_handle = world_interface_->get_entity_handle(entity);
    
    
    for (const auto& callback : callback_list_.callbacks())
    {
        auto cp_handle = get_control_point_handle(control_point);

        callback(engine_, decl::IControlPointListener_onControlPointHit, entity_handle, cp_handle, time_point);
    }
}

void ts::script::Control_point_interface::on_control_area_enter(world::Entity* entity, const world::Control_point* control_point, double time_point)
{
}

void ts::script::Control_point_interface::on_control_area_leave(world::Entity* entity, const world::Control_point* control_point, double time_point)
{

}