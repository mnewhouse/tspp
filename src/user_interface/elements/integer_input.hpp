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

#ifndef GUI_INTEGER_INPUT_HPP
#define GUI_INTEGER_INPUT_HPP

#include "../stylable_element.hpp"

#include "text_element.hpp"

namespace ts
{
    namespace gui
    {
        struct Integer_input_style
        {
            Vector2<double> text_area_size;
            Vector2<double> arrow_area_size;

            gui::Text_style text_style;
            gui::Text_style text_hover_style;
        };

        struct Integer_input_range
        {
            std::int32_t min = 0;
            std::int32_t max = -1;
            std::int32_t value = 0;
            std::int32_t increment = 1;
        };

        class Integer_input
            : public Stylable_element<Integer_input_style>
        {
        public:
            Integer_input(Integer_input_style style, Integer_input_range range);

            void increment(std::int32_t delta = 1);
            void decrement(std::int32_t delta = 1);

            void set_value(std::int32_t value);
            std::int32_t value() const;

            struct on_value_change_t {};
            static const on_value_change_t on_value_change;

        private:
            void update_value(std::int32_t value);
            std::int32_t get_increment_delta() const;

            virtual void apply_style(const Integer_input_style& style) override {}

            Integer_input_range range_;

            gui::Text_element* numeric_text_;
            gui::Text_element* increment_arrow_;
            gui::Text_element* decrement_arrow_;
        };

        template <>
        struct Event_traits<Integer_input::on_value_change_t>
        {
            using function_type = void(class Integer_input&, std::int32_t new_value);
        };
    }
}

#endif