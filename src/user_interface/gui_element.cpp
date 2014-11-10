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
#include "gui_element.hpp"
#include "document.hpp"

const ts::utf8_string ts::gui::states::hover = "__HOVER";
const ts::utf8_string ts::gui::states::selected = "__SELECTED";
const ts::utf8_string ts::gui::states::inactive = "__INACTIVE";

ts::gui::Element::Element(Vector2<double> size)
{
    set_size(size);
}

ts::gui::Element::Element(Vector2<double> size, relative_t relative)
{
    set_size(size, relative);
}

void ts::gui::Element::set_element_id(utf8_string id)
{
    auto document = owner_document();
    if (document)
    {
        document->register_element_id(this, id);
    }

    element_id_ = std::move(id);
}

void ts::gui::Element::set_size(Vector2<double> size)
{
    if (std::abs(size.x - size_.x) >= 0.01 || std::abs(size.y - size_.y) >= 0.01 || relative_sizing_)
    {
        size_ = size;
        relative_sizing_ = false;

        set_auto_sizing(size.x == 0.0 || size.y == 0.0);

        dirty_layout();
    }
}

void ts::gui::Element::set_state(const State_name& state_name, bool new_state)
{
    auto it = std::find(active_states_.begin(), active_states_.end(), state_name);
    auto found = it != active_states_.end();

    if (found)
    {
        active_states_.erase(it);
    }

    if (new_state)
    {
        active_states_.push_back(state_name);
    }

    if (new_state != found)
    {
        state_change();
    }
}

bool ts::gui::Element::is_state_set(const State_name& state_name) const
{
    return std::find(active_states_.begin(), active_states_.end(), state_name) != active_states_.end();
}

void ts::gui::Element::state_change()
{
    auto apply_func = [this](const Background_style& style) { apply_style(style); };

    if (active_states_.empty())
    {
        styler_.state_change(apply_func);
    }

    else
    {
        const auto& states = active_states_;
        auto it = std::find_if(states.rbegin(), states.rend(), [this](const State_name& state_name)
        {
            return styler_.has_style(state_name);
        });

        if (it != states.rend())
        {
            styler_.state_change(*it, apply_func);
        }

        else
        {
            styler_.state_change(apply_func);
        }
    }

    on_state_change(active_states_);
}

void ts::gui::Element::set_size(Vector2<double> size, relative_t)
{
    if (size_ != size || !relative_sizing_)
    {
        size_ = size;
        relative_sizing_ = true;

        set_auto_sizing(size.x == 0.0 || size.y == 0.0);

        dirty_layout();
    }
}

ts::Vector2<double> ts::gui::Element::size() const
{
    update_layout_if_dirty();

    return layout_.size;
}

void ts::gui::Element::set_auto_sizing(bool enable)
{
    if (enable != automatic_sizing_)
    {
        automatic_sizing_ = enable;

        dirty_layout();
    }
}

bool ts::gui::Element::auto_sizing() const
{
    return automatic_sizing_;
}

void ts::gui::Element::center_horizontal(bool center)
{
    if (center_horizontal_ != center)
    {
        center_horizontal_ = center;

        dirty_layout();
    }
}

void ts::gui::Element::center_vertical(bool center)
{
    if (center_vertical_ != center)
    {
        center_vertical_ = center;
        dirty_layout();
    }
}

bool ts::gui::Element::is_centered_horizontally() const
{
    return center_horizontal_;
}

bool ts::gui::Element::is_centered_vertically() const
{
    return center_vertical_;
}


bool ts::gui::Element::visible() const
{
    return state_.visible;
}

void ts::gui::Element::set_visible(bool visible)
{
    if (visible != state_.visible)
    {
        state_.visible = visible;

        dirty_layout();

        reset_mouse_state();
    }
}

void ts::gui::Element::set_active(bool active)
{
    if (active != state_.active)
    {
        state_.active = active;

        set_state(states::inactive, !active);

        dirty_layout();
    }
}

bool ts::gui::Element::active() const
{
    return state_.active;
}

bool ts::gui::Element::hover() const
{
    return state_.hover;
}

void ts::gui::Element::set_offset(Vector2<double> offset)
{
    if (offset_ != offset)
    {
        offset_ = offset;

        dirty_layout();
    }
}

ts::Vector2<double> ts::gui::Element::offset() const
{
    return offset_;
}


void ts::gui::Element::set_position(Vector2<double> position)
{
    if (std::abs(position_.x - position.x) >= 0.01 || std::abs(position_.y - position.y) >= 0.01 || relative_positioning_)
    {
        position_ = position;
        relative_positioning_ = false;

        dirty_layout();
    }
}

void ts::gui::Element::set_position(Vector2<double> position, relative_t)
{
    if (std::abs(position_.x - position.x) >= 0.001 || std::abs(position_.y - position.y) >= 0.001 || !relative_positioning_)
    {
        position_ = position;
        relative_positioning_ = true;

        dirty_layout();
    }
}

ts::Vector2<double> ts::gui::Element::position() const
{
    update_layout_if_dirty();

    return layout_.position;
}

void ts::gui::Element::apply_style(const Background_style& style)
{
    background_ = style.background;
}

void ts::gui::Element::set_background_style(const Background_style& style)
{
    styler_.set_default_style(style);

    state_change();
}

void ts::gui::Element::register_background_style(State_name state, const Background_style& style)
{
    styler_.register_style(std::move(state), style);
}

void ts::gui::Element::update()
{
}

const std::vector<std::unique_ptr<ts::gui::Element>>& ts::gui::Element::children() const
{
    return children_;
}

void ts::gui::Element::enable_clipping(bool enable)
{
    clipping_enabled_ = enable;
}

bool ts::gui::Element::is_clipping_enabled() const
{
    return clipping_enabled_;
}

void ts::gui::Element::override_clipping(bool enable)
{
    override_clipping_ = enable;
}

bool ts::gui::Element::is_clipping_overridden() const
{
    return override_clipping_;
}

const ts::gui::Element* ts::gui::Element::root_element() const
{
    auto elem = this;
    while (elem->parent_)
    {
        elem = elem->parent_;
    }

    return elem;
}

ts::gui::Element* ts::gui::Element::root_element()
{
    auto elem = this;
    while (elem->parent_)
    {
        elem = elem->parent_;
    }

    return elem;
}

void ts::gui::Element::update_hierarchy() const
{
    const_cast<Element&>(*this).update_layout();
}

void ts::gui::Element::render_hierarchy(sf::RenderTarget& render_target, sf::RenderStates render_states) const
{
    update_layout_if_dirty();

    render(render_target, render_states);
}

void ts::gui::Element::render(sf::RenderTarget& render_target, sf::RenderStates render_states) const
{
    if (!visible()) return;

    const auto my_pos = position();
    stored_view_ = render_target.getView();

    render_states.transform.translate(static_cast<float>(my_pos.x), static_cast<float>(my_pos.y));

    if (is_clipping_enabled())
    {
        auto my_size = size();
        
        sf::Vector2f sf_size(static_cast<float>(my_size.x), static_cast<float>(my_size.y));
        sf::Vector2f top_left = render_states.transform.transformPoint(0.0f, 0.0f);

        sf::Vector2f viewport_top_left = stored_view_.getTransform().transformPoint(top_left);
        sf::Vector2f viewport_bottom_right = stored_view_.getTransform().transformPoint(top_left + sf_size);
        
        viewport_top_left.x = (viewport_top_left.x + 1.0f) * 0.5f;
        viewport_top_left.y = (-viewport_top_left.y + 1.0f) * 0.5f;

        viewport_bottom_right.x = (viewport_bottom_right.x + 1.0f) * 0.5f;
        viewport_bottom_right.y = (-viewport_bottom_right.y + 1.0f) * 0.5f;

        auto old_viewport = stored_view_.getViewport();

        sf::FloatRect viewport(viewport_top_left, viewport_bottom_right - viewport_top_left);
        viewport.left = old_viewport.left + viewport.left * old_viewport.width;
        viewport.top = old_viewport.top + viewport.top * old_viewport.height;
        viewport.width *= old_viewport.width;
        viewport.height *= old_viewport.height;

        // Little hack to make sure the absolute viewport size isn't rounded down
        viewport.width += 0.5f / render_target.getSize().x;
        viewport.height += 0.5f / render_target.getSize().y;

        auto clipped_view = stored_view_;
        clipped_view.reset(sf::FloatRect(top_left, sf_size));
        clipped_view.setViewport(viewport);

        render_target.setView(clipped_view);
    }

    if (is_clipping_overridden())
    {
        auto& view = root_element()->stored_view_;

        render_target.setView(root_element()->stored_view_);
    }

    if (background_)
    {
        background_->draw(*this, render_target, render_states);
    }

    render_states.transform.translate(static_cast<float>(offset_.x), static_cast<float>(offset_.y));

    draw(render_target, render_states);

    for (const auto& child : children_)
    {
        child->render_hierarchy(render_target, render_states);     
    }

    // Restore the old view
    render_target.setView(stored_view_);
}

void ts::gui::Element::destroy_child(Element* child)
{
    auto it = std::find_if(children_.begin(), children_.end(),
        [child](const std::unique_ptr<Element>& child_ptr)
    {
        return &*child_ptr == child;
    });

    if (it != children_.end())
    {
        if (callback_guard_count_ != 0)
        {
            std::size_t index = std::distance(children_.begin(), it);
            destroyed_children_.insert(index);
        }

        else
        {
            children_.erase(it);
        }

        dirty_layout();
    }
}

void ts::gui::Element::destroy()
{
    if (parent_)
    {
        parent_->destroy_child(this);
    }
}

void ts::gui::Element::clear_children()
{
    if (callback_guard_count_ != 0)
    {
        clear_children_ = true;
    }

    else
    {
        children_.clear();

        dirty_layout();
    }
}

void ts::gui::Element::release_destroyed_children()
{
    if (clear_children_)
    {
        children_.clear();

        clear_children_ = false;

        dirty_layout();
    }

    else if (!destroyed_children_.empty())
    {
        for (auto index : destroyed_children_)
        {
            children_.erase(children_.begin() + index);
        }

        dirty_layout();
    }

    destroyed_children_.clear();
}

void ts::gui::Element::adopt_pending_children()
{
    auto size = children_.size();
    children_.resize(size + pending_children_.size());

    std::move(pending_children_.begin(), pending_children_.end(), children_.begin() + size);

    pending_children_.clear();
}

const ts::gui::Element* ts::gui::Element::parent() const
{
    return parent_;
}

ts::gui::Element* ts::gui::Element::parent()
{
    return parent_;
}

ts::gui::Document* ts::gui::Element::owner_document()
{
    if (!parent_)
    {
        return nullptr;
    }

    if (!owner_document_)
    {
        owner_document_ = parent_->owner_document();
    }

    return owner_document_;
}

void ts::gui::Element::dirty_layout()
{
    auto root = root_element();
    root->layout_.dirty = true;
}

void ts::gui::Element::update_layout_if_dirty() const
{
    auto root = const_cast<Element*>(root_element());

    if (root->layout_.dirty)
    {
        root->update_layout();
    }
}

void ts::gui::Element::process_text_input(utf8::uint32_t code_point)
{
    Callback_guard guard(this);

    trigger_event(events::on_text_input, *this, code_point);

    for (auto& child : children_)
    {
        if (child->active())
        {
            child->process_text_input(code_point);
        }
    }
}

void ts::gui::Element::process_key_down(sf::Keyboard::Key key, unsigned int modifiers)
{
    Callback_guard guard(this);

    trigger_event(events::on_key_down, *this, key, modifiers);

    for (auto& child : children_)
    {
        if (child->active())
        {
            child->process_key_down(key, modifiers);
        }
    }
}

void ts::gui::Element::process_key_up(sf::Keyboard::Key key, unsigned int modifiers)
{
    Callback_guard guard(this);

    trigger_event(events::on_key_up, *this, key, modifiers);

    for (auto& child : children_)
    {
        if (child->active())
        {
            child->process_key_up(key, modifiers);
        }
    }
}

void ts::gui::Element::reset_mouse_state()
{
    set_state(states::hover, false);

    state_.hover = false;
    state_.click = 0;

    for (auto& child : children_)
    {
        child->reset_mouse_state();
    }
}

void ts::gui::Element::process_mouse_wheel(int delta, unsigned int key_modifiers)
{
    Callback_guard guard(this);

    trigger_event(events::on_mouse_wheel, *this, delta, key_modifiers);

    for (auto& child : children_)
    {
        if (child->active())
        {
            child->process_mouse_wheel(delta, key_modifiers);
        }
    }
}

void ts::gui::Element::process_mouse_move(Vector2<double> old_position, Vector2<double> new_position)
{
    Double_rect rect;
    rect.left = 0.0;
    rect.top = 0.0;
    rect.width = -1.0;
    rect.height = -1.0;

    process_mouse_move(old_position, new_position, rect);
}

void ts::gui::Element::process_mouse_move(Vector2<double> old_position, Vector2<double> new_position, Double_rect clip_area)
{
    Callback_guard guard(this);

    auto my_position = position();
    old_position -= my_position;
    new_position -= my_position;

    clip_area.left -= my_position.x;
    clip_area.top -= my_position.y;

    if (is_clipping_enabled())
    {
        auto my_size = size();

        clip_area.left = 0.0;
        clip_area.top = 0.0;
        clip_area.width = my_size.x;
        clip_area.height = my_size.y;
    }

    if (is_clipping_overridden())
    {
        clip_area.left = 0.0;
        clip_area.top = 0.0;
        clip_area.width = -1.0;
        clip_area.height = -1.0;
    }

    auto hover_state = hover(new_position);
    if (clip_area.width >= 0.0 && clip_area.height >= 0.0)
    {
        hover_state = hover_state && contains(clip_area, new_position);
    }

    if (hover_state != state_.hover)
    {
        state_.hover = hover_state;
        set_state(states::hover, hover_state);
    }

    trigger_event(events::on_mouse_move, *this, old_position, new_position);

    auto my_offset = offset();

    old_position -= my_offset;
    new_position -= my_offset;

    clip_area.left -= my_offset.x;
    clip_area.top -= my_offset.y;

    for (const auto& child : children_)
    {
        if (child->active())
        {
            child->process_mouse_move(old_position, new_position, clip_area);
        }
    }    
}

void ts::gui::Element::process_mouse_button_down(mouse::Button button, Vector2<double> mouse_position)
{
    Callback_guard guard(this);

    mouse_position -= position();
    auto button_id = static_cast<unsigned int>(button);

    if (state_.hover)
    {
        state_.click |= button_id;

        trigger_event(events::on_mouse_down, *this, button);
    }

    else if (state_.click & button_id)
    {
        state_.click ^= button_id;
    }

    mouse_position -= offset();

    for (const auto& child : children_)
    {
        if (child->active())
        {
            child->process_mouse_button_down(button, mouse_position);
        }
    }
}

void ts::gui::Element::process_mouse_button_up(mouse::Button button, Vector2<double> mouse_position)
{
    Callback_guard guard(this);

    mouse_position -= position();
    auto button_id = static_cast<unsigned int>(button);

    if (state_.click & button_id)
    {
        // Click event
        if (button_id == mouse::Button::Left && state_.hover)
        {
            trigger_event(events::on_click, *this);
        }

        state_.click ^= button_id; 
    }

    trigger_event(events::on_mouse_up, *this, button);

    mouse_position -= offset();

    for (const auto& child : children_)
    {
        if (child->active())
        {
            child->process_mouse_button_up(button, mouse_position);
        }
    }
}

bool ts::gui::Element::hover(Vector2<double> mouse_position) const
{
    return mouse_position.x >= 0.0 && mouse_position.x < layout_.size.x && 
           mouse_position.y >= 0.0 && mouse_position.y < layout_.size.y;
}

void ts::gui::Element::draw(sf::RenderTarget& render_target, sf::RenderStates render_states) const
{
    // Draw nothing by default
}

void ts::gui::Element::on_state_change(const State_list& states)
{
}

void ts::gui::Element::on_layout()
{
}

ts::Vector2<double> ts::gui::Element::content_size() const
{
    return child_bounds();
}

ts::Vector2<double> ts::gui::Element::child_bounds() const
{
    Vector2<double> bounds;

    for (auto& child : children_)
    {
        if (!child->visible()) continue;

        auto child_bounds = child->position() + child->size();

        if (child_bounds.x > bounds.x) bounds.x = child_bounds.x;
        if (child_bounds.y > bounds.y) bounds.y = child_bounds.y;
    }

    return bounds;
}

void ts::gui::Element::update_layout()
{
    layout_.dirty = false;

    perform_layout_update();
    apply_centering();

    layout_event();
}

void ts::gui::Element::layout_event()
{
    for (auto& child : children_)
    {
        child->layout_event();
    }

    on_layout();
}

void ts::gui::Element::perform_layout_update()
{
    auto old_size = layout_.size;

    layout_.size = size_;
    if (relative_sizing_ && parent_)
    {
        layout_.size *= parent_->layout_.size;
    }

    layout_.position = position_;
    if (relative_positioning_ && parent_)
    {
        layout_.position *= parent_->layout_.size;
    }

    for (auto& child : children_)
    {
        child->perform_layout_update();
    }

    for (auto& child : pending_children_)
    {
        child->perform_layout_update();
    }

    if (automatic_sizing_)
    {
        layout_.size = content_size();
    }

    if (old_size != layout_.size)
    {
        trigger_event(events::on_resize, *this, old_size, layout_.size);
    }

    state_change();
}

void ts::gui::Element::apply_centering()
{
    if (center_horizontal_ && parent_)
    {
        layout_.position.x = std::floor((parent_->layout_.size.x - layout_.size.x) * 0.5);
    }

    if (center_vertical_ && parent_)
    {
        layout_.position.y = std::floor((parent_->layout_.size.y - layout_.size.y) * 0.5);
    }

    for (auto& child : children_)
    {
        child->apply_centering();
    }
}

ts::gui::Element::Callback_guard::Callback_guard(Element* element)
: element_(element)
{
    ++element->callback_guard_count_;
}


ts::gui::Element::Callback_guard::~Callback_guard()
{
    --element_->callback_guard_count_;

    if (element_->callback_guard_count_ == 0)
    {
        element_->release_destroyed_children();
        element_->adopt_pending_children();
    }
}