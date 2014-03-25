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

#ifndef STATES_ACTION_HPP
#define STATES_ACTION_HPP

#include "user_interface/gui_state.hpp"
#include "user_interface/gui_scene.hpp"

#include "game/track_builder.hpp"
#include "game/camera.hpp"
#include "game/drawable_entity.hpp"
#include "game/particle_generator.hpp"
#include "game/stage_data.hpp"

#include "world/world.hpp"
#include "world/entity_listener.hpp"

#include "controls/key_mapping.hpp"
#include "controls/control_center.hpp"

#include "audio/car_sounds.hpp"
#include "audio/collision_sounds.hpp"



namespace ts
{

    namespace cup
    {
        struct Stage_data;
    }


    namespace states
    {

        class Action_scene
            : public graphics::Render_scene, public world::Entity_listener
        {
        public:
            Action_scene(const resources::Track& track, const game::Stage_data& stage_data);

            virtual void render(graphics::Render_target& render_target) override;
            void update(std::size_t frame_duration);

            void zoom_in();
            void zoom_out();

            game::Camera& camera();
            game::Particle_generator& particle_generator();

            virtual void on_car_create(world::Car* car) override;
            virtual void on_entity_destroy(world::Entity* entity) override;

        private:
            game::Track_builder_result build_track(const cup::Stage_data& stage_data);

            game::Track_builder_result track_components_;

            std::vector<game::Drawable_entity> drawable_entities_;

            sf::Clock frame_time_;
            double frame_duration_;

            game::Camera camera_;
            game::Particle_generator particle_generator_;
        };

        class Action_state
            : public gui::State
        {
        public:
            Action_state(resources::Track&& track, const game::Stage_data& stage_data, 
                         const Handle<state_machine_type>& state_machine, const Handle<gui::Context>& context,
                         std::shared_ptr<resources::Resource_store> resource_store);

            virtual void render(graphics::Render_target& render_target) override;
            virtual void handle_event(const sf::Event& event) override;
            virtual void update(std::size_t frame_duration) override;

        private:
            Action_scene action_scene_;

            world::World world_;

            controls::Control_center control_center_;
            controls::Key_mapping key_mapping_;

            audio::Car_sound_controller car_sound_controller_;
            audio::Collision_sound_controller collision_sound_controller_;
        };

    }

}

#endif