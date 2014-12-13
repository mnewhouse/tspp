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

#ifndef CORE_GAME_STATE_HPP
#define CORE_GAME_STATE_HPP

#include "core/state_machine.hpp"
#include "graphics/render_scene.hpp"
#include "resources/resource_store.hpp"

namespace sf
{
    class Event;
}

namespace ts
{
    namespace game
    {
        

        // class Game_state
        // Abstract base class for a client game state. This is essential to the game logic,
        // as all the events and the rendering goes through here. It declares various virtual functions
        // that can be overriden in derived classes for customization.
        class Game_state
        {
        public:
            using state_machine_type = core::State_machine<Game_state>;

            Game_state(state_machine_type* state_machine, resources::Resource_store* resource_store)
                : state_machine_(state_machine),
                  resource_store_(resource_store)
            {}

            virtual ~Game_state() {}

            state_machine_type* state_machine() const { return state_machine_; }

            void render(sf::RenderTarget& render_target, double frame_time);

            void add_render_scene(graphics::Render_scene* render_scene);

            virtual void update(std::size_t frame_duration) {}
            virtual void handle_event(const sf::Event& event) {}

            void activate() { on_activate(); };

            resources::Resource_store* resource_store() const { return resource_store_; }

        private:
            virtual void on_activate() {};

            state_machine_type* state_machine_;
            resources::Resource_store* resource_store_;

            std::vector<graphics::Render_scene*> render_scenes_;
        };

    }

}

#endif