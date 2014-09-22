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
#include "text_element.hpp"

ts::gui::Text_element::Text_element(utf8_string text, const Text_style& style)
: Stylable_element(style),
  text_item_(std::move(text), style.font, style.character_size)
{
    apply_style_impl(style);
}

ts::gui::Text_element::Text_element(utf8_string text, const Styler_type& styler)
: Stylable_element(styler),
  text_item_(std::move(text), styler.default_style().font, styler.default_style().character_size)
{
    apply_style_impl(styler.default_style());
}

void ts::gui::Text_element::set_text(utf8_string text)
{
    text_item_.set_text(std::move(text));

    dirty_layout();
}

void ts::gui::Text_element::draw(sf::RenderTarget& render_target, sf::RenderStates render_states) const
{
    render_states.transform.translate(static_cast<float>(offset_.x), static_cast<float>(offset_.y));

    render_target.draw(text_item_.geometry(), render_states);
}

void ts::gui::Text_element::apply_style(const Text_style& style)
{
    apply_style_impl(style);
}

void ts::gui::Text_element::apply_style_impl(const Text_style& style)
{
    text_item_.set_color(style.color);
    text_item_.set_character_size(style.character_size);
    text_item_.set_font(style.font);
}

ts::Vector2<double> ts::gui::Text_element::content_size() const
{
    const auto& bounds = text_item_.bounds();
    return { bounds.right(), bounds.bottom() };
}

const ts::graphics::Text& ts::gui::Text_element::text_item() const
{
    return text_item_;
}

const ts::utf8_string& ts::gui::Text_element::text() const
{
    return text_item_.text();
}

void ts::gui::Text_element::apply_centering()
{
    offset_.x = 0.0;
    offset_.y = 0.0;

    if (center_horizontal_ || center_vertical_)
    {
        const auto& bounds = text_item_.bounds();

        auto my_size = size();

        if (center_horizontal_)
        {
            offset_.x = std::floor((my_size.x - bounds.width) * 0.5);
        }

        if (center_vertical_)
        {
            offset_.y = std::floor((my_size.y - bounds.height) * 0.5);
        }
    }
}

void ts::gui::Text_element::on_layout()
{
    apply_centering();
}

void ts::gui::Text_element::center_text_horizontal(bool center)
{
    if (center != center_horizontal_)
    {
        center_horizontal_ = center;

        dirty_layout();
    }    
}

void ts::gui::Text_element::center_text_vertical(bool center)
{
    if (center != center_vertical_)
    {
        center_vertical_ = center;

        dirty_layout();
    }
}