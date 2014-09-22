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

#include "stdinc.hpp"
#include "progress_bar.hpp"

ts::gui::Progress_bar::Progress_bar(Vector2i size, Progress_bar_style style)
: Stylable_element(style, size),
  bar_area_(create_child<Element>(size)),
  completed_part_(create_child<Element>())
{
    apply_style_impl(style);
}

double ts::gui::Progress_bar::value() const
{
    return value_;
}

double ts::gui::Progress_bar::max_value() const
{
    return max_value_;
}

void ts::gui::Progress_bar::set_value(double value)
{
    value = std::min(value, max_value_);
    value = std::max(value, 0.0);

    update_progress(current_style(), value);

    value_ = value;
}

void ts::gui::Progress_bar::update_progress(const Progress_bar_style& style, double value)
{
    Vector2<double> my_size = size();

    auto progress = 0.0;
    if (max_value_ > 0.0)
    {
        progress = value / max_value_;
    }

    auto padding = style.padding;

    auto progress_size = my_size;
    progress_size.x = (my_size.x - (padding * 2.0)) * progress;
    progress_size.y -= padding * 2.0;

    completed_part_->set_size(progress_size);
    completed_part_->set_position({ padding, padding });
}

void ts::gui::Progress_bar::set_max_value(double max_value)
{
    max_value_ = max_value;

    value_ = std::min(value_, max_value_);
}

void ts::gui::Progress_bar::apply_style(const Progress_bar_style& style)
{
    apply_style_impl(style);
}

void ts::gui::Progress_bar::apply_style_impl(const Progress_bar_style& style)
{
    bar_area_->set_background_style(style.bar_style);

    completed_part_->set_background_style(style.completed_style);
    completed_part_->set_position({ style.padding, style.padding });

    update_progress(style, value());
}