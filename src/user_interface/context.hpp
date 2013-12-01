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

#ifndef GUI_CONTEXT_HPP
#define GUI_CONTEXT_HPP

#include "mouse_state.hpp"

#include <memory>
#include <string>

namespace ts
{

    namespace gui
    {

        typedef std::size_t element_id;

        struct Element_state
        {
            bool hot;
            bool active;
            bool clicked;
        };

        class Context
        {
        public:
            Context();

            element_id allocate_base_id();

            const mouse::State& mouse_state() const;
            element_id hot_item() const;
            element_id active_item() const;

            void set_hot_item(element_id id);
            void set_active_item(element_id id);
            void set_mouse_state(const mouse::State& new_state);

            void reset();

        private:
            mouse::State mouse_state_;
            element_id next_id_;

            element_id hot_item_;
            element_id active_item_;
            std::string keyboard_input_;
        };

    }

}

#endif