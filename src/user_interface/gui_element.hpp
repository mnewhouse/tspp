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

#include "event_manager.hpp"
#include "element_style.hpp"
#include "background_style.hpp"
#include "mouse.hpp"

#include <SFML/Graphics.hpp>

namespace ts
{
    namespace gui
    {
        static struct relative_t {} relative;

        namespace states
        {
            extern const utf8_string hover;
            extern const utf8_string selected;
            extern const utf8_string inactive;
        }

        class Document;

        class Element
        {
        public:
            using State_name = utf8_string;
            using State_list = std::vector<State_name>;

            Element(Vector2<double> size = {});
            Element(Vector2<double> size, relative_t);

            template <typename ElemType, typename... Args>
            ElemType* create_child(Args&&... args);

            const std::vector<std::unique_ptr<Element>>& children() const;

            const Element* parent() const;
            Element* parent();
            virtual Document* owner_document();
            const Element* root_element() const;
            Element* root_element();

            void render_hierarchy(sf::RenderTarget& render_target, sf::RenderStates render_states = sf::RenderStates::Default) const;
            void update_hierarchy() const;

            void destroy_child(Element* child);

            void destroy();
            void update();

            bool hover() const;

            void set_size(Vector2<double> size);
            void set_size(Vector2<double> size, relative_t);
            Vector2<double> size() const;

            void set_position(Vector2<double> position);
            void set_position(Vector2<double> position, relative_t);
            Vector2<double> position() const;

            void set_offset(Vector2<double> offset);
            Vector2<double> offset() const;

            void set_auto_sizing(bool enable);
            bool auto_sizing() const;

            void process_mouse_move(Vector2<double> old_position, Vector2<double> new_position);
            void process_mouse_button_down(mouse::Button button, Vector2<double> position);
            void process_mouse_button_up(mouse::Button button, Vector2<double> position);

            void process_mouse_wheel(int delta, unsigned int modifiers);

            void process_text_input(utf8::uint32_t code_point);
            void process_key_down(sf::Keyboard::Key key, unsigned int key_modifiers);
            void process_key_up(sf::Keyboard::Key key, unsigned int key_modifiers);

            void set_element_id(utf8_string id);
            const utf8_string& element_id() const;

            template <typename EventType, typename FuncType>
            Event_handler<EventType> add_event_handler(EventType, FuncType func);

            template <typename EventType>
            void remove_event_handler(Event_handler<EventType> event_handler);

            void set_state(const State_name& state, bool active);
            bool is_state_set(const State_name& state) const;

            using Background_styler_type = Element_style<State_name, Background_style>;

            void register_background_style(State_name state_name, const Background_style& style);
            void set_background_style(const Background_style& default_style);

            void set_visible(bool visible);
            bool visible() const;

            void set_active(bool active);
            bool active() const;

            void enable_clipping(bool enable = true);
            bool is_clipping_enabled() const;

            void override_clipping(bool enable = true);
            bool is_clipping_overridden() const;

            void center_horizontal(bool center = true);
            void center_vertical(bool center = true);

            bool is_centered_horizontally() const;
            bool is_centered_vertically() const;

            Vector2<double> child_bounds() const;

            void clear_children();

        protected:
            template <typename EventType, typename... Args>
            void trigger_event(EventType, Args&&... args);

            void dirty_layout();

        private:
            void render(sf::RenderTarget& render_target, sf::RenderStates render_states) const;

            virtual bool hover(Vector2<double> mouse_position) const;
            virtual void draw(sf::RenderTarget& render_target, sf::RenderStates render_states) const;
            virtual Vector2<double> content_size() const;

            virtual void on_layout();
            virtual void on_state_change(const State_list& states);

            void process_mouse_move(Vector2<double> old_position, Vector2<double> new_position, Double_rect clip_area);

            void apply_style(const Background_style& style);
            void layout_event();

            void state_change();
            void child_layout(Element& child);

            void update_layout_if_dirty() const;
            void update_layout();
            void perform_layout_update();
            void apply_centering();

            void release_destroyed_children();
            void adopt_pending_children();

            void reset_mouse_state();

            bool relative_sizing_ = false;
            bool relative_positioning_ = false;
            
            bool automatic_sizing_ = true;
            bool center_horizontal_ = false;
            bool center_vertical_ = false;

            bool clipping_enabled_ = false;
            bool override_clipping_ = false;

            bool clear_children_ = false;

            std::size_t callback_guard_count_ = 0;

            Vector2<double> position_;
            Vector2<double> size_;
            Vector2<double> offset_;

            struct Callback_guard
            {
                Callback_guard(Element* element);
                ~Callback_guard();

            private:
                Element* element_;
            };

            struct Element_state
            {
                bool hover = false;            
                bool visible = true;
                bool active = true;
                unsigned int click = 0;
            } state_;

            mutable struct Layout
            {
                bool dirty = true;
                Vector2<double> position;
                Vector2<double> size;
                Vector2<double> padding;
            } layout_;

            Document* owner_document_ = nullptr;
            Element* parent_ = nullptr;
            std::vector<std::unique_ptr<Element>> children_;
            std::vector<std::unique_ptr<Element>> pending_children_;
            std::set<std::size_t, std::greater<std::size_t>> destroyed_children_;

            utf8_string element_id_;
            State_list active_states_;

            Event_manager event_manager_;
            Background_styler_type styler_;

            Background background_;

            // For internal use only.
            mutable sf::View stored_view_;
        };

        namespace events
        {
            struct on_click_t {};
            static const on_click_t on_click;

            struct on_mouse_down_t {};
            static const on_mouse_down_t on_mouse_down;

            struct on_mouse_up_t {};
            static const on_mouse_up_t on_mouse_up;

            struct on_mouse_move_t {};
            static const on_mouse_move_t on_mouse_move;

            struct on_mouse_wheel_t {};
            static const on_mouse_wheel_t on_mouse_wheel;

            struct on_resize_t {};
            static const on_resize_t on_resize;

            struct on_text_input_t {};
            static const on_text_input_t on_text_input;

            struct on_key_down_t {};
            static const on_key_down_t on_key_down;

            struct on_key_up_t {};
            static const on_key_up_t on_key_up;
        }

        template <>
        struct Event_traits<events::on_click_t>
        {
            using function_type = void(class Element&);
        };

        template <>
        struct Event_traits<events::on_mouse_down_t>
        {
            using function_type = void(class Element&, mouse::Button button_id);
        };

        template <>
        struct Event_traits<events::on_mouse_up_t>
        {
            using function_type = void(class Element&, mouse::Button button_id);
        };

        template <>
        struct Event_traits<events::on_mouse_move_t>
        {
            using function_type = void(class Element&, Vector2i old_position, Vector2i new_position);
        };

        template <>
        struct Event_traits<events::on_mouse_wheel_t>
        {
            using function_type = void(class Element&, int delta, unsigned int key_modifiers);
        };

        template <>
        struct Event_traits<events::on_resize_t>
        {
            using function_type = void(class Element&, Vector2<double> old_size, Vector2<double> new_size);
        };

        template <>
        struct Event_traits<events::on_text_input_t>
        {
            using function_type = void(class Element&, utf8::uint32_t code_point);
        };

        template <>
        struct Event_traits<events::on_key_down_t>
        {
            using function_type = void(class Element&, sf::Keyboard::Key key, unsigned int modifiers);
        };

        template <>
        struct Event_traits<events::on_key_up_t>
        {
            using function_type = void(class Element&, sf::Keyboard::Key key, unsigned int modifiers);
        };
    }
}

template <typename EventType, typename FuncType>
ts::gui::Event_handler<EventType> ts::gui::Element::add_event_handler(EventType event_type, FuncType func)
{
    return event_manager_.add_event_handler(event_type, func);
}

template <typename EventType>
void ts::gui::Element::remove_event_handler(Event_handler<EventType> event_handler)
{
    event_manager_.remove_event_handler(event_handler);
}

template <typename EventType, typename... Args>
void ts::gui::Element::trigger_event(EventType event_type, Args&&... args)
{
    event_manager_.trigger_event(event_type, std::forward<Args>(args)...);
}

template <typename ElemType, typename... Args>
ElemType* ts::gui::Element::create_child(Args&&... args)
{
    auto child = std::make_unique<ElemType>(std::forward<Args>(args)...);

    child->parent_ = this;
    ElemType* result = &*child;

    if (callback_guard_count_ != 0)
    {
        pending_children_.push_back(std::move(child));
    }

    else
    {
        children_.push_back(std::move(child));
    }
    

    return result;
}


#endif