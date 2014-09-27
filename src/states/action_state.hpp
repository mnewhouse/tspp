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

#ifndef STATES_ACTION_HPP
#define STATES_ACTION_HPP

#include "game/cup.hpp"
#include "game/action_loader.hpp"

#include "user_interface/gui_state.hpp"

#include "world/world_listener.hpp"

namespace ts
{
    namespace controls
    {
        class Control_interface;
    }

    namespace states
    {
        class Action_state_base
            : public gui::State, public world::World_listener
        {
        public:
            Action_state_base(game::Loaded_scene loaded_scene, state_machine_type* state_machine,
                              gui::Context* context, resources::Resource_store* resource_store);

            virtual ~Action_state_base();

            virtual void handle_event(const sf::Event& event) override;
            virtual void update(std::size_t frame_duration) override;

            virtual void on_car_create(world::Car* car) override;
            virtual void on_car_destroy(world::Car* car) override;

            virtual void on_collision(const world::Collision_result& collision) override;

        private:
            virtual void on_activate() override;

            std::unique_ptr<world::World> world_;
            std::unique_ptr<game::Action_scene> action_scene_;
            std::unique_ptr<controls::Control_center> control_center_;
            std::unique_ptr<audio::Sound_controller> sound_controller_;
            std::unique_ptr<script_api::Client_interface> client_script_interface_;
            std::unique_ptr<controls::Control_interface> control_interface_;
        };
    }
}

#endif