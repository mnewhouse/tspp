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

#ifndef SCRIPT_WORLD_INTERFACE_HPP
#define SCRIPT_WORLD_INTERFACE_HPP

#include "callback.hpp"

#include "world/world_listener.hpp"

#include <angelscript.h>

#include <unordered_map>
#include <set>

namespace ts
{
    namespace world
    {
        class Entity;
        class World;
    }

    namespace script
    {
        class Engine;
        class World_interface;

        struct Entity_handle
        {
            world::Entity* entity;
            World_interface* world_interface;
            std::size_t ref_count;
            bool alive;
        };

        struct Entity_iterator
        {
            std::size_t index;
            World_interface* interface;
        };

        const std::size_t WorldInterface_udata = 0x80000;

        class World_interface
            : public world::World_listener
        {
        public:
            explicit World_interface(world::World* world, Engine* engine);

            virtual void on_tick(std::size_t ticks) override;
            virtual void on_update() override;
            virtual void on_start() override;
            virtual void on_entity_destroy(world::Entity* entity) override;

            void addEventListener(asIScriptObject* object);
            void removeEventListener(asIScriptObject* object);

            Entity_handle* get_entity_handle(world::Entity* entity);
            void release_entity_handle(Entity_handle* entity_handle);

            bool is_entity_alive(world::Entity* entity);

            world::World* world() const;

            Entity_iterator* create_entity_iterator();
            void release_entity_iterator(Entity_iterator* entity_iterator);


        private:
            Engine* engine_;
            Callback_list callback_list_;
            world::World* world_;

            std::unordered_map<world::Entity*, Entity_handle> entity_handles_;
        };
    }
}


#endif