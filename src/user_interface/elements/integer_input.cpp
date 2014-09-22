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
#include "integer_input.hpp"

const ts::gui::Integer_input::on_value_change_t ts::gui::Integer_input::on_value_change;

ts::gui::Integer_input::Integer_input(Integer_input_style style, Integer_input_range range)
: Stylable_element(style),
  range_(range),
  numeric_text_(create_child<gui::Text_element>("", style.text_style)),
  increment_arrow_(create_child<gui::Text_element>(">", style.text_style)),
  decrement_arrow_(create_child<gui::Text_element>("<", style.text_style))
{
    increment_arrow_->set_size(style.arrow_area_size);
    increment_arrow_->center_text_horizontal();

    decrement_arrow_->set_size(style.arrow_area_size);
    decrement_arrow_->center_text_horizontal();

    increment_arrow_->register_style(gui::states::hover, style.text_hover_style);
    decrement_arrow_->register_style(gui::states::hover, style.text_hover_style);

    numeric_text_->set_size(style.text_area_size);
    numeric_text_->set_position({ style.arrow_area_size.x, 0.0 });
    numeric_text_->center_text_horizontal();

    increment_arrow_->set_position({ style.arrow_area_size.x + style.text_area_size.x, 0.0 });

    increment_arrow_->add_event_handler(gui::events::on_click, 
        [this](const gui::Element& element)
    {
        increment();
    });

    decrement_arrow_->add_event_handler(gui::events::on_click,
        [this](const gui::Element& element)
    {
        decrement();
    });

    auto value = clamp(range.value, range.min, range.max);
    update_value(value);
}


void ts::gui::Integer_input::increment(std::int32_t delta)
{
    set_value(value() + delta);
}

void ts::gui::Integer_input::decrement(std::int32_t delta)
{
    set_value(value() - delta);
}

void ts::gui::Integer_input::set_value(std::int32_t value)
{
    value = clamp(value, range_.min, range_.max);

    if (value != range_.value)
    {
        update_value(value);
    }
}

void ts::gui::Integer_input::update_value(std::int32_t value)
{
    numeric_text_->set_text(std::to_string(value));
    range_.value = value;
    trigger_event(on_value_change, *this, value);
}

std::int32_t ts::gui::Integer_input::value() const
{
    return range_.value;
}