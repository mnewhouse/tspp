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

#include "world_listener.hpp"
#include "collision_bitmap.hpp"
#include "collisions.hpp"
#include "control_point_manager.hpp"
#include "game_timer.hpp"

#include "resources/track.hpp"
#include "resources/terrain_map.hpp"

#include "core/vector2.hpp"

#include <memory>
#include <vector>
#include <deque>
#include <unordered_set>

namespace ts
{
    namespace resources
    {
        struct Car_definition;
        class Track;
    }

    namespace world
    {
        struct Entity_listener;
        struct Entity_state;

        class Entity;        
        class Car;       

        class World
        {
        public:
            explicit World(resources::Track&& track);

            Car* create_car(const resources::Car_definition& car_def);

            void add_world_listener(World_listener* world_listener);

            void update(std::size_t frame_duration);

            const resources::Terrain_definition& terrain_at(Vector2d point) const;
            const resources::Terrain_definition& terrain_at(Vector2d point, std::size_t level) const;

            const std::shared_ptr<Collision_bitmap>& collision_bitmap
                (const std::shared_ptr<resources::Pattern>& pattern);

            const resources::Track& track() const;
            const std::vector<std::unique_ptr<Car>>& car_list() const;

            Control_point_manager& control_point_manager();

            void launch_game();

            void start_game_timer();
            std::size_t game_time() const;

            bool is_entity(Entity* entity) const;
            const std::vector<Entity*>& entity_list() const;

            

        private:
            void register_entity(Entity* entity);

            void handle_collisions(double frame_duration);

            void load_track_objects();

            Vector2f world_size_;

            std::vector<std::unique_ptr<Car>> car_list_;

            std::vector<Entity*> entity_list_;
            std::unordered_set<Entity*> entity_set_;            

            std::vector<World_listener*> world_listeners_;

            std::vector<std::pair<Entity*, Vector2<double>>> position_buffer_;
            std::vector<Entity_state> state_buffer_;
            std::deque<Collision_result> collision_queue_;

            resources::Track track_;
            resources::Pattern terrain_map_;
            Static_collision_bitmap scenery_bitmap_;

            Control_point_manager control_point_manager_;

            Collision_bitmap_store dynamic_bitmap_store_;
            Game_timer game_timer_;
            bool started_ = false;
        };

    }

}

#endif