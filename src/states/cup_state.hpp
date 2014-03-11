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

#ifndef STATES_CUP_STATE_HPP
#define STATES_CUP_STATE_HPP

#include "cup/cup.hpp"

#include "user_interface/gui_state.hpp"

#include <future>
#include <memory>

namespace ts
{
    namespace states
    {
        class Action_state;

        class Cup_state
            : public gui::State, public cup::Cup
        {
        public:
            Cup_state(const Handle<state_machine_type>& state_machine,
                      const Handle<gui::Context>& context);

            void async_load();
            bool is_loading() const;
            bool is_loading_finished() const;

            void launch_action();

        private:
            std::future<std::unique_ptr<Action_state>> future_state_;
        };
    }
}

#endif
