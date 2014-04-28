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

#ifndef GUI_CONTEXT_HPP
#define GUI_CONTEXT_HPP

#include "mouse_state.hpp"

#include <memory>
#include <string>

#include <unordered_set>

namespace ts
{
    namespace gui
    {
        class Element;

        class Context
        {
        public:
            Context(Vector2u screen_size);

            const mouse::State& mouse_state() const;
            void set_mouse_state(const mouse::State& new_state);

            const Vector2u& screen_size() const;

        private:
            mouse::State mouse_state_;

            std::string keyboard_input_;
            Vector2u screen_size_;
        };
    }
}

#endif