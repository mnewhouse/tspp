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

#ifndef GUI_STATE_HPP
#define GUI_STATE_HPP

#include "graphics/render_scene.hpp"

#include "core/game_state.hpp"

#include <memory>

namespace ts
{
    namespace gui
    {
        class Context;

        class State
            : public core::Game_state
        {
        public:
            State(state_machine_type* state_machine, Context* context,
                  resources::Resource_store* resource_store);

            void set_background(const graphics::Background& background);
            const graphics::Background& background() const;

            Context* context() const;

        protected:
            void render_background(graphics::Render_target& render_target);

        private:
            graphics::Background background_;
            Context* context_;
        };

    }
}

#endif