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

#ifndef SCRIPT_CONTROL_POINT_INTERFACE_HPP
#define SCRIPT_CONTROL_POINT_INTERFACE_HPP

#include "world/control_point_listener.hpp"
#include "world/control_point_manager.hpp"

#include "callback.hpp"

#include <unordered_map>

class asIScriptObject;

namespace ts
{
    namespace script
    {
        class Engine;
        class Control_point_interface;
        class World_interface;

        struct Control_point_handle
        {
            const world::Control_point* control_point;
            Control_point_interface* cp_interface;            
            std::size_t ref_count;
            bool dynamic;
        };

        const std::size_t ControlPointInterface_udata = 0x120000;

        class Control_point_interface
            : public world::Control_point_listener
        {
        public:
            Control_point_interface(world::Control_point_manager* cp_manager, World_interface* world_interface, Engine* engine);

            void addEventListener(asIScriptObject* object);
            void removeEventListener(asIScriptObject* object);
            
            Control_point_handle* create_control_point(Vector2i start, Vector2i end);

            Control_point_handle* get_control_point_handle(const world::Control_point* control_point);
            void release_control_point_handle(const Control_point_handle* control_point);

            Control_point_handle* get_control_point_by_id(std::int32_t id);
            std::int32_t get_control_point_id(const world::Control_point* control_point) const;
            std::uint32_t get_control_point_count() const;

            void set_entity_control_point(world::Entity* entity, const world::Control_point* control_point);
            void ignore_control_points_for_entity(world::Entity* entity);
                
            virtual void on_control_point_hit(world::Entity* entity, const world::Control_point* control_point, double time_point);
            virtual void on_control_area_enter(world::Entity* entity, const world::Control_point* control_point, double time_point);
            virtual void on_control_area_leave(world::Entity* entity, const world::Control_point* control_point, double time_point);

        private:
            world::Control_point_manager* cp_manager_;
            std::unordered_map<const world::Control_point*, Control_point_handle> cp_handles_;
            Callback_list callback_list_;
            
            Engine* engine_;
            World_interface* world_interface_;
        };
    }
}

#endif