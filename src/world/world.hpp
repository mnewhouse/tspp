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

#ifndef WORLD_WORLD_HPP
#define WORLD_WORLD_HPP

#include "collision_bitmap.hpp"
#include "collisions.hpp"

#include "resources/track.hpp"
#include "resources/terrain_map.hpp"

#include "core/vector2.hpp"

#include <memory>
#include <vector>
#include <deque>

namespace ts
{
    namespace resources
    {
        struct Car_definition;
        class Track;
    }

    namespace world
    {
        class Entity_listener;
        class Entity;
        struct Entity_state;
        class Car;        

        class World
        {
        public:
            explicit World(resources::Track&& track);

            Car* create_car(const resources::Car_definition& car_def);

            void add_entity_listener(Entity_listener* entity_listener);

            void update(std::size_t frame_duration);

            const resources::Terrain_definition& terrain_at(Vector2d point) const;
            const resources::Terrain_definition& terrain_at(Vector2d point, std::size_t level) const;

        private:
            Vector2d clamp_position(Vector2d position) const;

            void handle_collisions(double frame_duration);

            void load_track_objects();

            Vector2f world_size_;

            std::vector<std::unique_ptr<Car>> car_list_;
            std::vector<Entity*> entity_list_;

            std::vector<Entity_listener*> entity_listeners_;
                

            std::vector<Entity_state> state_buffer_;
            std::deque<Collision_result> collision_queue_;

            resources::Track track_;
            resources::Pattern terrain_map_;
            Static_collision_bitmap scenery_bitmap_;
        };

    }

}

#endif