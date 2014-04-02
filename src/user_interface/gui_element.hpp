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

#ifndef GUI_ELEMENT_HPP
#define GUI_ELEMENT_HPP

#include "mouse_state.hpp"
#include "core/vector2.hpp"
#include "graphics/renderer.hpp"

#include <memory>
#include <vector>

namespace ts
{
    namespace gui
    {
        struct Element_state
        {
            bool hover = false;
            bool pressed = false;
            bool clicked = false;
            bool focus = false;
        };

        class Element
        {
        public:
            Element() = default;
            virtual ~Element() = default;

            Element(Element&& elem);
            Element& operator=(Element&& elem);

            void set_focus(bool focus);
            bool focus() const;

            void set_visible(bool visible);
            bool visible() const;

            void set_position(std::int32_t x, std::int32_t y);
            void set_position(Vector2i position);
            const Vector2i& position() const;

            void set_size(std::int32_t width, std::int32_t height);
            void set_size(Vector2i size);
            const Vector2i& size() const;

            Element_state element_state() const;

            void reset_state();

            void render(graphics::Render_target& render_target, graphics::Render_states render_states) const;
            void update(mouse::State mouse_state);

            template <typename ElementType, typename... Args>
            ElementType* create_child(Args&&... args);

            const std::vector<std::unique_ptr<Element>>& children() const;
            void destroy();

            bool clicked() const;
            bool pressed() const;
            bool hover() const;
            
        private:
            virtual bool hover(Vector2i mouse_position) const;
            virtual void draw(graphics::Render_target& render_target, graphics::Render_states render_states) const;
            virtual void on_state_change(Element_state old_state, Element_state new_state);
            virtual void on_resize(Vector2i old_size, Vector2i new_size);

            bool visible_ = true;
            Element_state element_state_;
            Vector2i position_;
            Vector2i size_;
            std::vector<std::unique_ptr<Element>> children_;

            Element* parent_ = nullptr;
        };
    }
}

template <typename ElementType, typename... Args>
ElementType* ts::gui::Element::create_child(Args&&... args)
{
    auto child_elem = std::make_unique<ElementType>(std::forward<Args>(args)...);
    auto child_ptr = &*child_elem;
    child_elem->parent_ = this;

    children_.push_back(std::move(child_elem));
    
    return child_ptr;
}

#endif