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

#include "gui_element.hpp"

ts::gui::Element::Element(Element&& elem)
: element_state_(elem.element_state_),
  visible_(elem.visible_),
  position_(elem.position_),
  children_(std::move(elem.children_)),
  parent_(elem.parent_)
{
    elem.parent_ = nullptr;
}

ts::gui::Element& ts::gui::Element::operator=(Element&& elem)
{
    element_state_ = elem.element_state_;
    position_ = elem.position_;
    children_ = std::move(elem.children_);
    parent_ = elem.parent_;
    visible_ = elem.visible_;

    elem.parent_ = nullptr;
    elem.visible_ = false;
    return *this;
}

void ts::gui::Element::update(mouse::State mouse_state)
{
    if (!visible()) return;

    mouse_state.position -= position_;

    bool button_state = ((mouse_state.button_state & mouse::Button::Left) != 0);

    Element_state new_state;
    new_state.hover = hover(mouse_state.position);
    new_state.clicked = !button_state && element_state_.pressed;
    new_state.pressed = element_state_.hover && button_state;

    if (new_state.hover != element_state_.hover ||
        new_state.clicked != element_state_.clicked ||
        new_state.pressed != element_state_.pressed)
    {
        on_state_change(element_state_, new_state);
        element_state_ = new_state;
    }

    
    for (auto& child_elem : children_)
    {
        child_elem->update(mouse_state);
    }
}

void ts::gui::Element::render(graphics::Render_target& render_target, graphics::Render_states render_states) const
{
    if (!visible()) return;

    render_states.transform.translate(position_.x, position_.y);

    draw(render_target, render_states);

    for (const auto& child_elem : children_)
    {
        child_elem->render(render_target, render_states);
    }
}

void ts::gui::Element::destroy()
{
    if (parent_)
    {
        auto& parent_children = parent_->children_;

        auto pred = [this](const std::unique_ptr<Element>& elem) { return &*elem == this; };
        auto it = std::find_if(parent_children.begin(), parent_children.end(), pred);

        parent_children.erase(it);
    }
}

void ts::gui::Element::reset_state()
{
    auto new_state = Element_state();
    if (element_state_.clicked || element_state_.hover || element_state_.pressed || element_state_.focus)
    {
        on_state_change(element_state_, new_state);
        element_state_ = new_state;
    }
}

const std::vector<std::unique_ptr<ts::gui::Element>>& ts::gui::Element::children() const
{
    return children_;
}

bool ts::gui::Element::clicked() const
{
    return element_state_.clicked;
}

bool ts::gui::Element::pressed() const
{
    return element_state_.pressed;
}

bool ts::gui::Element::hover() const
{
    return element_state_.hover;
}

bool ts::gui::Element::visible() const
{
    return visible_;
}

void ts::gui::Element::set_visible(bool visible)
{
    visible_ = visible;
}

ts::gui::Element_state ts::gui::Element::element_state() const
{
    return element_state_;
}

const ts::Vector2i& ts::gui::Element::position() const
{
    return position_;
}

void ts::gui::Element::set_position(std::int32_t x, std::int32_t y)
{
    position_.x = x;
    position_.y = y;
}

void ts::gui::Element::set_position(Vector2i position)
{
    position_ = position;
}

const ts::Vector2i& ts::gui::Element::size() const
{
    return size_;
}

void ts::gui::Element::set_size(std::int32_t width, std::int32_t height)
{
    size_.x = width;
    size_.y = height;
}

void ts::gui::Element::set_size(Vector2i size)
{
    on_resize(size_, size);

    size_ = size;
}

void ts::gui::Element::draw(graphics::Render_target& render_target, graphics::Render_states render_states) const
{
}

bool ts::gui::Element::hover(Vector2i mouse_position) const
{
    return false;
}

void ts::gui::Element::on_state_change(Element_state old_state, Element_state new_state)
{
}

void ts::gui::Element::on_resize(Vector2i old_size, Vector2i new_size)
{
}