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

#include "world_interface.hpp"
#include "script_engine.hpp"

#include "world/world.hpp"
#include "script_definitions.hpp"

#include <angelscript.h>

#include <algorithm>

ts::script::World_interface::World_interface(world::World* world, Engine* engine)
    : world_(world), engine_(engine)
{
}

void ts::script::World_interface::on_tick(std::size_t ticks)
{
    auto context = engine_->context();

    for (const auto& callback : callback_list_.callbacks())
    {
        callback(engine_, decl::IWorldListener_onTick, ticks);
    }
}

void ts::script::World_interface::on_update()
{
    for (const auto& callback : callback_list_.callbacks())
    {
        callback(engine_, decl::IWorldListener_onUpdate);
    }
}

void ts::script::World_interface::on_start()
{
    for (const auto& callback : callback_list_.callbacks())
    {
        callback(engine_, decl::IWorldListener_onStart);
    }
}

void ts::script::World_interface::on_entity_destroy(world::Entity* entity)
{
    auto it = entity_handles_.find(entity);
    if (it != entity_handles_.end())
    {
        it->second.alive = false;
    }
}

void ts::script::World_interface::removeEventListener(asIScriptObject* listener)
{
    listener->Release();

    callback_list_.remove(listener);
}

void ts::script::World_interface::addEventListener(asIScriptObject* listener)
{
    callback_list_.store(listener);

    listener->Release();
}

ts::script::Entity_handle* ts::script::World_interface::get_entity_handle(world::Entity* entity)
{
    auto it = entity_handles_.find(entity);
    if (it != entity_handles_.end()) return &it->second;

    Entity_handle handle;
    handle.entity = entity;
    handle.world_interface = this;
    handle.ref_count = 1;
    handle.alive = is_entity_alive(entity);

    auto result = entity_handles_.insert(std::make_pair(entity, handle));
    return &result.first->second;
}

void ts::script::World_interface::release_entity_handle(Entity_handle* entity_handle)
{
    entity_handles_.erase(entity_handle->entity);
}

bool ts::script::World_interface::is_entity_alive(world::Entity* entity)
{
    return world_->is_entity(entity);
}

ts::world::World* ts::script::World_interface::world() const
{
    return world_;
}