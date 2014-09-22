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
#include "scroll_pane.hpp"

ts::gui::Scroll_pane::Scroll_pane(Vector2<double> size, const Scroll_pane_style& default_style)
: Stylable_element(default_style, size),
  vertical_scroll_bar_(),
  area_size_(-1.0, -1.0)
{
    enable_clipping();

    create_vertical_scroll_bar(size);

    set_area_size({});

    apply_style_impl(default_style);
}

void ts::gui::Scroll_pane::create_vertical_scroll_bar(Vector2<double> size)
{
    auto scroll_bar_size = size;
    scroll_bar_size.x = current_style().vertical_scroll_bar_width;

    vertical_scroll_bar_ = create_child<Slider_bar>(scroll_bar_size, Slider_direction::Vertical, Slider_range{ 0.0, 1.0 });

    vertical_scroll_bar_->add_event_handler(events::on_slider_move, [this](Slider_bar& slider_bar, double new_value)
    {
        vertical_scroll(new_value);
    });

    vertical_scroll_bar_->add_event_handler(events::on_mouse_wheel, 
        [this](Element& element, int delta, unsigned int key_modifiers)
    {
        handle_scroll_wheel(delta);
    });

    vertical_scroll_bar_->set_position({ size.x, 0.0 });

    vertical_scroll_bar_->override_clipping();
}

void ts::gui::Scroll_pane::on_layout()
{
    Vector2<double> bounds;

    for (auto& child : children())
    {
        if (&*child == vertical_scroll_bar_) continue;

        auto child_bounds = child->position() + child->size();

        if (child_bounds.x > bounds.x) bounds.x = child_bounds.x;
        if (child_bounds.y > bounds.y) bounds.y = child_bounds.y;
    }

    set_area_size(bounds);
}

void ts::gui::Scroll_pane::set_area_size(Vector2<double> area_size)
{
    auto my_size = size();

    if (area_size != area_size_)
    {
        area_size_ = area_size;

        if (area_size.y > my_size.y)
        {            
            vertical_scroll_bar_->set_visible(true);

            auto slider_size = vertical_scroll_bar_->slider()->size();
            slider_size.y = my_size.y * (my_size.y / area_size.y);

            vertical_scroll_bar_->slider()->set_size(slider_size);

            vertical_scroll_bar_->update_position();
        }

        else
        {
            vertical_scroll(0.0);
            vertical_scroll_bar_->set_visible(false);
        }        
    }
}

void ts::gui::Scroll_pane::handle_scroll_wheel(int delta)
{
    if (vertical_scroll_bar_->visible())
    {
        auto my_size = size();

        auto max_offset = area_size_.y - my_size.y;
        auto offset = (vertical_scroll_bar_->value() * max_offset) - (delta * 10);

        vertical_scroll_bar_->set_value(offset / max_offset);
    }
}

void ts::gui::Scroll_pane::vertical_scroll(double new_value)
{
    auto my_size = size();
    auto new_offset = offset();

    auto max_offset = area_size_.y - my_size.y;
    new_offset.y = new_value * -max_offset;

    set_offset(new_offset);

    vertical_scroll_bar_->set_position({ my_size.x, -new_offset.y });
}

ts::gui::Slider_bar* ts::gui::Scroll_pane::vertical_scroll_bar() const
{
    return vertical_scroll_bar_;
}

void ts::gui::Scroll_pane::apply_style(const Scroll_pane_style& style)
{
    apply_style_impl(style);
}

void ts::gui::Scroll_pane::apply_style_impl(const Scroll_pane_style& style)
{
    auto vertical_bar_size = vertical_scroll_bar_->size();

    vertical_bar_size.x = style.vertical_scroll_bar_width;
    vertical_scroll_bar_->set_size(vertical_bar_size);
}
