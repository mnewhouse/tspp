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

#include "game/loaded_scene.hpp"

#include "user_interface/gui_state.hpp"

#include "world/world_listener.hpp"
#include "cup/cup_listener.hpp"

namespace ts
{
    namespace controls
    {
        class Event_translator;
        class Control_interface;
    }

    namespace messages
    {
        class Message;
    }

    namespace states
    {
        class Action_state_base
            : public gui::State, public world::World_listener, public cup::Cup_listener
        {
        public:
            Action_state_base(game::Loaded_scene loaded_scene, controls::Control_interface* control_interface,
                              state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store);

            virtual ~Action_state_base();

            virtual void handle_event(const sf::Event& event) override;
            virtual void update(std::size_t frame_duration) override;

            virtual void on_car_create(world::Car* car) override;
            virtual void on_car_destroy(world::Car* car) override;

            virtual void on_collision(const world::Collision_result& collision) override;

        private:
            virtual void on_activate() override;
            virtual void on_state_change(cup::Cup_state old_state, cup::Cup_state new_state);

            virtual void end_action() = 0;

            game::Loaded_scene scene_;

            std::unique_ptr<controls::Event_translator> control_event_translator_;
            controls::Control_interface* control_interface_;
        };
    }
}

#endif