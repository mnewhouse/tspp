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

#ifndef GUI_PROGRESS_BAR_HPP
#define GUI_PROGRESS_BAR_HPP

#include "../stylable_element.hpp"

namespace ts
{
    namespace gui
    {
        struct Progress_bar_style
        {
            double padding = 1.0;

            Background_style bar_style;
            Background_style completed_style;
        };

        class Progress_bar
            : public Stylable_element<Progress_bar_style>
        {
        public:
            Progress_bar(Vector2i size, Progress_bar_style style);

            void set_max_value(double max_value);
            double max_value() const;

            void set_value(double value);
            double value() const;

        private:
            void update_progress(const Progress_bar_style& style, double value);

            virtual void apply_style(const Progress_bar_style& style) override;
            void apply_style_impl(const Progress_bar_style& style);

            gui::Element* bar_area_;
            gui::Element* completed_part_;

            double value_ = 0.0;
            double max_value_ = 1.0;
        };
    }
}

#endif
