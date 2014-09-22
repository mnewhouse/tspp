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
#include "slider_bar.hpp"

ts::gui::Slider_bar::Slider_bar(Vector2<double> elem_size, Slider_direction direction, Slider_range range)
: Element(elem_size),
  slider_(make_slider(elem_size, direction)),
  direction_(direction),
  range_(range)
{
    set_auto_sizing(false);
}

void ts::gui::Slider_bar::update_position()
{
    set_value(value());
}

void ts::gui::Slider_bar::set_value(double value)
{
    auto my_size = size();
    auto slider_size = slider_->size();
    auto slider_position = slider_->position();

    auto max_offset = my_size - slider_size;
    auto range_length = range_.end - range_.start;

    value_ = clamp(value, range_.start, range_.end);

    if (direction_ == Slider_direction::Vertical)
    {
        slider_position.y = max_offset.y * (value_ / range_length);
    }

    else
    {
        slider_position.x = max_offset.x * (value_ / range_length);
    }

    slider_->set_position(slider_position);

    trigger_event(events::on_slider_move, *this, value_);
}

double ts::gui::Slider_bar::value() const
{
    return value_;
}

ts::gui::Slider_range ts::gui::Slider_bar::range() const
{
    return range_;
}

void ts::gui::Slider_bar::create_drag_listener(Element& slider)
{
    auto drag_callback = [this](Element& slider, Vector2i old_position, Vector2i new_position)
    {
        on_drag(slider, old_position, new_position);
    };

    drag_listener_ = slider.add_event_handler(events::on_mouse_move, drag_callback);
}

ts::gui::Slider* ts::gui::Slider_bar::slider() const
{
    return slider_;
}

void ts::gui::Slider_bar::remove_drag_listener(Element& slider)
{
    slider.remove_event_handler(drag_listener_);
    drag_listener_ = {};
}

void ts::gui::Slider_bar::on_drag(Element& slider, Vector2i old_position, Vector2i new_position)
{
    auto my_size = size();
    auto slider_position = slider.position();
    auto slider_size = slider.size();

    auto max_offset = my_size - slider_size;

    auto new_value = range_.start;
    auto range_length = range_.end - range_.start;

    auto delta = new_position - old_position;

    if (direction_ == Slider_direction::Horizontal)
    {
        if (max_offset.x != 0.0)
        {
            new_value += range_length * ((slider_position.x + delta.x) / max_offset.x);
        }
    }
    
    else
    {
        if (max_offset.y != 0.0)
        {
            new_value += range_length * ((slider_position.y + delta.y) / max_offset.y);
        }
    }

    slider_->set_position(slider_position);

    set_value(new_value);
}

ts::gui::Slider* ts::gui::Slider_bar::make_slider(Vector2<double> elem_size, Slider_direction direction)
{
    // The slider is a square by default.
    auto slider = create_child<Slider>();
    if (direction == Slider_direction::Horizontal)
    {
        slider->set_size({ elem_size.y, elem_size.y });
    }

    else
    {
        slider->set_size({ elem_size.x, elem_size.x });
    }

    slider->set_visible(true);

    slider->add_event_handler(events::on_mouse_down, [this](Element& element, mouse::Button button) 
    {
        if (button == mouse::Button::Left)
        {
            create_drag_listener(element);
        }
    });

    slider->add_event_handler(events::on_mouse_up, [this](Element& element, mouse::Button button)
    {
        if (button == mouse::Button::Left)
        {
            remove_drag_listener(element);
        }
    });

    return slider;
}