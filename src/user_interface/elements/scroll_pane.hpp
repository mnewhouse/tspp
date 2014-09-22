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

#ifndef GUI_SCROLL_PANE_HPP
#define GUI_SCROLL_PANE_HPP

#include "../stylable_element.hpp"
#include "slider_bar.hpp"

namespace ts
{
    namespace gui
    {
        struct Scroll_pane_style
        {
            double vertical_scroll_bar_width = 16.0;
            double horizontal_scroll_bar_height = 16.0;
        };

        class Scroll_pane
            : public Stylable_element<Scroll_pane_style>
        {
        public:
            Scroll_pane(Vector2<double> size, const Scroll_pane_style& default_style);

            Slider_bar* vertical_scroll_bar() const;

        private:
            virtual void apply_style(const Scroll_pane_style& style) override;
            void apply_style_impl(const Scroll_pane_style& style);

            void create_vertical_scroll_bar(Vector2<double> size);

            virtual void on_layout() override;
            
            void set_area_size(Vector2<double> area_size);
            void vertical_scroll(double new_value);

            void handle_scroll_wheel(int delta);

            Slider_bar* vertical_scroll_bar_;
            Vector2<double> area_size_;
        };
    }
}

#endif