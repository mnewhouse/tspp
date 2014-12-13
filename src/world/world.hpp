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

#ifndef WORLD_WORLD_HPP
#define WORLD_WORLD_HPP

#include "world_listener.hpp"
#include "collision_bitmap.hpp"
#include "collisions.hpp"
#include "control_point_manager.hpp"
#include "game_timer.hpp"

#include "resources/track_handle.hpp"
#include "resources/pattern.hpp"

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
        class Car;    

        class World
            : public core::Listener_host<World_listener>
        {
        public:
            explicit World(std::unique_ptr<resources::Track> track, resources::Pattern track_pattern);
            ~World();

            Car* create_car(const resources::Car_definition& car_def, std::uint16_t car_id);
            Car* create_car(const resources::Car_definition& car_def);

            void update(std::size_t frame_duration);
            static double max_entity_speed();

            const resources::Terrain_definition& terrain_at(Vector2i point) const;
            const resources::Terrain_definition& terrain_at(Vector2i point, std::size_t level) const;

            const resources::Track& track() const;
            const std::vector<std::unique_ptr<Car>>& car_list() const;

            Control_point_manager& control_point_manager();

            void launch_game();

            void start_game_timer();
            std::size_t game_time() const;
            std::size_t world_time() const;

            bool is_entity(Entity* entity) const;
            const std::vector<Entity*>& entity_list() const;

        private:
            void register_entity(Entity* entity);

            struct Entity_update_state
            {
                Entity* entity = nullptr;
                Entity* collided_with = nullptr;
                std::size_t collision_count = 0;

                Vector2i current_position;
                std::size_t level;
                std::size_t rotation_index;
            };

            struct Entity_step
            {
                Vector2i old_position;
                Vector2i new_position;
            };

            void displacement_step(Entity_update_state& entity, Vector2i old_position, Vector2i new_position);
            void terrain_transition(Entity_update_state& entity, const resources::Terrain_definition& old_terrain,
                                    const resources::Terrain_definition& new_terrain);

            void handle_collision(Entity* subject, Entity* object, const Collision_info& collision_info);
            bool entity_state_collides(const Entity_update_state& entity_state);
            bool resolve_recurring_collisions(Entity_update_state& subject_state, Entity_update_state& object_state, double frame_duration);

            void load_track_objects();

            std::vector<std::unique_ptr<Car>> car_list_;

            std::vector<Entity*> entity_list_;
            std::unordered_set<Entity*> entity_set_;

            std::vector<Entity_update_state> state_buffer_;
            std::vector<Entity_step> step_buffer_;

            std::unique_ptr<resources::Track> track_;
            resources::Pattern terrain_map_;
            Vector2<double> world_size_;

            Static_collision_bitmap scenery_bitmap_;

            Control_point_manager control_point_manager_;

            Game_timer game_timer_;
        };

    }

}

#endif