/*
 * Turbo Sliders++
 * Copyright (C) 2013 Martin Newhouse
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

#include "graphics/track_builder.hpp"
#include "graphics/camera.hpp"
#include "graphics/drawable_entity.hpp"

#include "world/world.hpp"
#include "world/entity_listener.hpp"

#include "controls/key_mapping.hpp"
#include "controls/control_center.hpp"




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
            Action_scene(const resources::Track& track, const cup::Stage_data& stage_data);

            virtual void render(graphics::Render_target& render_target) override;
            void update(std::size_t frame_duration);

            void zoom_in();
            void zoom_out();


            virtual void on_car_create(const world::Car& car) override;
            virtual void on_entity_destroy(const world::Entity& entity) override;

        private:
            graphics::Track_builder_result build_track(const cup::Stage_data& stage_data);

            graphics::Track_builder_result track_components_;

            std::vector<graphics::Drawable_entity> drawable_entities_;

            sf::Clock frame_time_;
            double frame_duration_;

            graphics::Camera camera_;
        };

        class Action_state
            : public gui::State
        {
        public:
            Action_state(resources::Track&& track, const cup::Stage_data& stage_data, 
                         const Handle<state_machine_type>& state_machine, const Handle<gui::Context>& context);

            virtual void render(graphics::Render_target& render_target) override;
            virtual void handle_event(const sf::Event& event) override;
            virtual void update(std::size_t frame_duration) override;

        private:
            Action_scene action_scene_;

            world::World world_;

            controls::Control_center control_center_;
            controls::Key_mapping key_mapping_;
        };

    }

}

#endif