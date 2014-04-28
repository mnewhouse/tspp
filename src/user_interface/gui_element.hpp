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

#ifndef GUI_ELEMENT_HPP
#define GUI_ELEMENT_HPP

#include "gui_decorator.hpp"
#include "gui_events.hpp"
#include "mouse_state.hpp"
#include "core/vector2.hpp"
#include "graphics/renderer.hpp"

#include <memory>
#include <vector>
#include <unordered_set>

namespace ts
{
    namespace gui
    {
        class Element;

        struct Event_onLeftClick {};
        struct Event_onRightClick {};

        template <>
        struct Event_traits<Event_onLeftClick>
        {
            using function_type = void(const Element& element, Vector2i mouse_position);
        };

        template <>
        struct Event_traits<Event_onRightClick>
        {
            using function_type = void(const Element& element, Vector2i mouse_position);
        };

        struct Event_onResize {};
        template <>
        struct Event_traits<Event_onResize>
        {
            using function_type = void(const Element& element, Vector2i old_size, Vector2i new_size);
        };

        class Element
        {
        public:
            void set_focus(bool focus);
            bool has_focus() const;

            void enable_clipping(bool enable);
            bool clipping_enabled() const;

            void set_visible(bool visible);
            bool visible() const;

            void set_offset(Vector2i offset);
            void set_offset(std::int32_t x, std::int32_t y);
            const Vector2i& offset() const;

            void set_position(std::int32_t x, std::int32_t y);
            void set_position(Vector2i position);
            const Vector2i& position() const;

            void set_size(std::int32_t width, std::int32_t height);
            void set_size(Vector2i size);
            Vector2i size() const;

            const Vector2i& content_size() const;

            void render(graphics::Render_target& render_target, graphics::Render_states render_states) const;
            void update(mouse::State mouse_state);

            void destroy();

            template <typename ElemType, typename... Args>
            ElemType* create_child(Args&&... args);

            const std::vector<std::unique_ptr<Element>>& children() const;

            bool clicked() const;
            bool pressed() const;
            bool hover() const;

            template <typename Decorator, typename... Args>
            Decorator* create_decorator(Args&&... args);

            void set_state(std::uint32_t state_identifier, bool state);
            bool get_state(std::uint32_t state_identifier) const;

            template <typename EventType, typename F>
            void add_event_handler(EventType event_type, F function);

            template <typename EventType, typename... Args>
            void trigger_event(EventType event_type, Args&&... args);

            bool button_state(mouse::Button button) const;
            
        private:
            void set_content_size(Vector2i content_size);
            void update_parent();
            void render_decorators(graphics::Render_target& render_target, graphics::Render_states render_states) const;

            virtual bool hover(const Vector2i mouse_position) const;

            virtual void on_mouse_move(Vector2i old_position, Vector2i new_position) {}
            virtual void on_mouse_enter() {}
            virtual void on_mouse_leave() {}
            virtual void on_click(mouse::Button button, Vector2i mouse_position) {}
            virtual void on_resize(Vector2i old_size, Vector2i new_size) {}
            virtual void on_content_resize(Vector2i old_size, Vector2i new_size) {}
            virtual void on_text_input(const std::string& input) const {}
            virtual void on_update(mouse::State mouse_state) {}

            virtual void draw(graphics::Render_target& render_target, graphics::Render_states render_states) const {}

            Vector2i position_;
            Vector2i size_;
            Vector2i content_size_;
            Vector2i offset_;

            bool hover_ = false;
            bool focus_ = false;
            bool visible_ = true;
            bool auto_size_ = true;
            bool clipping_enabled_ = false;

            Vector2i mouse_position_;
            std::uint32_t button_state_ = 0;
            Vector2i clip_area_;

            std::vector<std::unique_ptr<Element>> children_;
            std::vector<std::unique_ptr<Decorator>> decorators_;
            std::unordered_set<std::uint32_t> active_states_;
            Element* parent_ = nullptr;

            Event_manager event_manager_;
        };
    }
}

template <typename ElemType, typename... Args>
ElemType* ts::gui::Element::create_child(Args&&... args)
{    
    auto child = std::make_unique<ElemType>(std::forward<Args>(args)...);
    auto result = &*child;
    child->parent_ = this;
    children_.push_back(std::move(child));
    
    return result;
}

template <typename DecoratorType, typename... Args>
DecoratorType* ts::gui::Element::create_decorator(Args&&... args)
{
    auto decorator = std::make_unique<DecoratorType>(std::forward<Args>(args)...);
    auto result = &*decorator;
    decorators_.push_back(std::move(decorator));

    return result;
}

template <typename EventType, typename F>
void ts::gui::Element::add_event_handler(EventType event_type, F function)
{
    event_manager_.add_event_handler(event_type, function);
}

template <typename EventType, typename... Args>
void ts::gui::Element::trigger_event(EventType event_type, Args&&... args)
{
    event_manager_.trigger_event(event_type, std::forward<Args>(args)...);
}

#endif