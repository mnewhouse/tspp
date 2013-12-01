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

#ifndef CORE_GAME_STATE_HPP
#define CORE_GAME_STATE_HPP

#include <vector>

#include "state_machine.hpp"
#include "handle.hpp"
#include "graphics/renderer.hpp"

namespace sf
{
    class Event;
}

namespace ts
{
    namespace core
    {
        

        // class Game_state
        // Abstract base class for a client game state. This is essential to the game logic,
        // as all the events and the rendering goes through here. It declares various virtual functions
        // that can be overriden in derived classes for customization.
        class Game_state
        {
        public:
            using State_machine = State_machine<Game_state>;

            Game_state(const Handle<State_machine>& state_machine)
                : state_machine_(state_machine)
            {}

            virtual ~Game_state() {}

            const Handle<State_machine>& state_machine() const { return state_machine_; }

            virtual void render(graphics::Render_target& render_target) = 0;

            virtual void update(std::size_t frame_duration) {}
            virtual void handle_event(const sf::Event& event) {}

        private:
            Handle<State_machine> state_machine_;
        };

    }

}

#endif