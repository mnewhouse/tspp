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

#ifndef STATES_LOADING_SCREEN_HPP
#define STATES_LOADING_SCREEN_HPP

#include "user_interface/gui_state.hpp"
#include "user_interface/gui_scene.hpp"

#include "resources/resource_store.hpp"

#include <future>

namespace ts
{

    namespace states
    {

        class Loading_state;

        class Loading_scene
            : public gui::Scene
        {
        public:
            explicit Loading_scene(gui::Context* context);

            virtual void render(graphics::Render_target& render_target) override;
        };

        class Loading_state
            : public gui::State
        {
        public:
            Loading_state(state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store);

            virtual ~Loading_state();

            virtual void render(graphics::Render_target&) override;
            virtual void update(std::size_t frame_duration) override;


        private:
            std::future<void> async_load_resources();
            bool is_loading_finished() const;

            void display_main_menu();

            Loading_scene scene_;
            std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
            std::future<void> future_;
        };

    }

}

#endif