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

#ifndef STATES_MAIN_MENU_HPP
#define STATES_MAIN_MENU_HPP

#include "user_interface/gui_state.hpp"
#include "user_interface/gui_scene.hpp"

namespace ts
{

    namespace states
    {

        class Main_menu;

        class Main_menu_scene
            : public gui::Scene
        {
        public:

            using state_machine_type = gui::State::state_machine_type;

            Main_menu_scene(Main_menu* main_menu, const Handle<gui::Context>& context)
                : gui::Scene(context), main_menu_(main_menu)
            {}

            virtual void render(graphics::Render_target& render_target) override;

        private:
            Main_menu* main_menu_;
        };

        class Main_menu
            : public gui::State
        {
        public:
            Main_menu(const Handle<state_machine_type>& state_machine, const Handle<gui::Context>& gui_context);

            virtual void render(graphics::Render_target& render_target) override;
            virtual void update(std::size_t frame_duration) override;

        private:
            Main_menu_scene scene_;
        };

    }

}

#endif