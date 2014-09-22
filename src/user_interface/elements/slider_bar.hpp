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

#ifndef GUI_SLIDER_BAR_HPP
#define GUI_SLIDER_BAR_HPP

#include "../gui_element.hpp"

namespace ts
{
    namespace gui
    {
        enum class Slider_direction
        {
            Vertical,
            Horizontal
        };

        struct Slider_range
        {
            double start;
            double end;
        };

        namespace events
        {
            struct on_slider_move_t {};
            static const on_slider_move_t on_slider_move;
        }

        class Slider_bar;

        class Slider
            : public Element
        {
        public:
            Slider() = default;

        private:
        };

        class Slider_bar
            : public Element
        {
        public:
            Slider_bar(Vector2<double> elem_size, Slider_direction direction, Slider_range range);

            Slider* slider() const;

            double value() const;
            void set_value(double value);

            Slider_range range() const;


            void update_position();

        private:
            void create_drag_listener(Element& slider);
            void remove_drag_listener(Element& slider);

            void on_drag(Element& slider, Vector2i old_position, Vector2i new_position);

            Slider* make_slider(Vector2<double> elem_size, Slider_direction direction);

            Slider_direction direction_;
            Slider_range range_;      
            double value_ = 0.0;
            Slider* slider_;

            Event_handler<events::on_mouse_move_t> drag_listener_;
        };

        template <>
        struct Event_traits<events::on_slider_move_t>
        {
            using function_type = void(class Slider_bar& slider_bar, double new_value);
        };
    }
}

#endif