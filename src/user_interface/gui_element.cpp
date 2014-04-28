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

#include <iostream>

void ts::gui::Element::update(mouse::State mouse_state)
{
    if (!visible()) return;

    auto mouse_position = mouse_state.position;
    mouse_state.position -= position_;

    auto hover_state = hover(mouse_state.position);

    const auto left = static_cast<std::uint32_t>(mouse::Button::Left);
    const auto right = static_cast<std::uint32_t>(mouse::Button::Right);

    if (hover_state)
    {
        if ((button_state_ & left) && !(mouse_state.button_state & left))
        {
            trigger_event(Event_onLeftClick(), *this, mouse_state.position);
            on_click(mouse::Button::Left, mouse_state.position);
        }

        if ((button_state_ & right) && !(mouse_state.button_state & right))
        {
            trigger_event(Event_onRightClick(), *this, mouse_state.position);
            on_click(mouse::Button::Right, mouse_state.position);
        }       
    }

    button_state_ = mouse_state.button_state & (left | right);

    if (mouse_state.position != mouse_position_)
    {
        on_mouse_move(mouse_position_, mouse_state.position);
    }

    mouse_position_ = mouse_position - position_;

    if (hover_state != hover_)
    {
        if (hover_state) on_mouse_enter();
        else on_mouse_leave();

        hover_ = hover_state;
    }     

    on_update(mouse_state);
    mouse_state.position -= offset_;
    
    for (auto& child_elem : children_)
    {
        child_elem->update(mouse_state);
    }
}

void ts::gui::Element::render(graphics::Render_target& render_target, graphics::Render_states render_states) const
{
    if (!visible()) return;

    sf::View view = render_target.getView(), old_view = view;
    render_states.transform.translate(position_.x, position_.y);

    auto elem_size = size();

    if (clipping_enabled_)
    {
        auto old_viewport = view.getViewport();

        auto rect = render_states.transform.transformRect(sf::FloatRect(0, 0, elem_size.x, elem_size.y));
        auto viewport = view.getTransform().transformRect(rect);
        auto point = view.getTransform().transformPoint(rect.left, rect.top);   

        viewport.left = old_viewport.left + (old_viewport.width * (point.x + 1.0) * 0.5);
        viewport.top = old_viewport.top + (old_viewport.height * (-point.y + 1.0) * 0.5);
        viewport.width *= old_viewport.width * 0.5;
        viewport.height *= old_viewport.height * 0.5;
        
        view.reset(rect);
        view.setViewport(viewport);

        render_target.setView(view);
    }

    render_decorators(render_target, render_states);

    render_states.transform.translate(offset_.x, offset_.y);

    draw(render_target, render_states);

    for (const auto& child_elem : children_)
    {
        child_elem->render(render_target, render_states);
    }

    render_target.setView(old_view);
}

void ts::gui::Element::render_decorators(graphics::Render_target& render_target, graphics::Render_states render_states) const
{
    for (const auto& decorator : decorators_)
    {
        decorator->draw(*this, render_target, render_states);
    }
}

void ts::gui::Element::destroy()
{
    if (parent_)
    {
        auto& parent_children = parent_->children_;

        auto pred = [this](const std::unique_ptr<Element>& elem) { return &*elem == this; };

        auto it = std::find_if(parent_children.begin(), parent_children.end(), pred);
        if (it != parent_children.end()) parent_children.erase(it);
    }
}

const std::vector<std::unique_ptr<ts::gui::Element>>& ts::gui::Element::children() const
{
    return children_;
}

bool ts::gui::Element::hover() const
{
    return hover_;
}


bool ts::gui::Element::visible() const
{
    return visible_;
}

bool ts::gui::Element::button_state(mouse::Button button) const
{
    return (button_state_ & static_cast<std::uint32_t>(button)) != 0;
}

void ts::gui::Element::set_visible(bool visible)
{
    visible_ = visible;
}

const ts::Vector2i& ts::gui::Element::position() const
{
    return position_;
}

void ts::gui::Element::set_position(std::int32_t x, std::int32_t y)
{
    set_position(Vector2i(x, y));
}

void ts::gui::Element::set_position(Vector2i position)
{
    mouse_position_ -= position - position_;

    position_ = position;

    update_parent();
}

void ts::gui::Element::update_parent()
{
    if (parent_)
    {
        auto bounds = position_ + size_;

        auto new_content_size = parent_->content_size_;

        if (bounds.x > new_content_size.x) new_content_size.x = bounds.x;
        if (bounds.y > new_content_size.y) new_content_size.y = bounds.y;

        parent_->set_content_size(new_content_size);
    }
}

void ts::gui::Element::set_content_size(Vector2i content_size)
{
    if (content_size_ != content_size)
    {
        on_content_resize(content_size_, content_size);
        content_size_ = content_size;

        if (parent_)
        {
            auto bounds = position_ + content_size;

            auto parent_content_size = parent_->content_size_;

            if (bounds.x > parent_content_size.x) parent_content_size.x = bounds.x;
            if (bounds.y > parent_content_size.y) parent_content_size.y = bounds.y;

            parent_->set_content_size(parent_content_size);
        }
    }
}

void ts::gui::Element::set_offset(std::int32_t x, std::int32_t y)
{
    set_offset(Vector2i(x, y));
}

void ts::gui::Element::set_offset(Vector2i offset)
{
    offset_ = offset;
}

const ts::Vector2i& ts::gui::Element::offset() const
{
    return offset_;
}

ts::Vector2i ts::gui::Element::size() const
{
    if (auto_size_ && parent_) return parent_->size();

    return size_;
}

const ts::Vector2i& ts::gui::Element::content_size() const
{
    return content_size_;
}

void ts::gui::Element::enable_clipping(bool enable)
{
    clipping_enabled_ = enable;
}

bool ts::gui::Element::clipping_enabled() const
{
    return clipping_enabled_;
}

void ts::gui::Element::set_size(std::int32_t width, std::int32_t height)
{
    set_size(Vector2i(width, height));
}

void ts::gui::Element::set_size(Vector2i size)
{
    on_resize(size_, size);

    trigger_event(Event_onResize(), *this, size_, size);
    size_ = size;
    auto_size_ = false;

    update_parent();
}

void ts::gui::Element::set_state(std::uint32_t state_identifier, bool state)
{
    if (state)
    {
        active_states_.insert(state_identifier);
    }

    else 
    {
        active_states_.erase(state_identifier);
    }
}

bool ts::gui::Element::get_state(std::uint32_t state_identifier) const
{
    return active_states_.find(state_identifier) != active_states_.end();
}

bool ts::gui::Element::hover(Vector2i mouse_position) const
{
    auto elem_size = size();

    return mouse_position.x >= 0 && mouse_position.x < elem_size.x &&
        mouse_position.y >= 0 && mouse_position.y < elem_size.y;
}

