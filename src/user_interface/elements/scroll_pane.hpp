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

#ifndef GUI_ELEMENTS_SCROLL_PANE_HPP
#define GUI_ELEMENTS_SCROLL_PANE_HPP

#include "draggable.hpp"
#include "background_decorator.hpp"

#include "../gui_element.hpp"

#include <memory>

namespace ts
{
    namespace gui
    {
        namespace elements
        {
            class Scroll_bar_slider
                : public Draggable
            {
            };

            class Vertical_scroll_track
                : public Element
            {
            public:
                Vertical_scroll_track(Vector2i size, std::int32_t content_height)
                    : pane_height_(size.y), content_height_(content_height)
                {
                    set_size(size);

                    slider_ = create_child<Scroll_bar_slider>();

                    slider_->create_decorator<Background_color>(sf::Color(255, 255, 255, 100));

                    set_pane_height(size.y);
                    set_content_height(content_height);
                }

                void set_content_height(std::int32_t content_height)
                {
                    content_height_ = content_height;

                    adjust_proportions();                    
                }

                void set_pane_height(std::int32_t height)
                {
                    pane_height_ = height;

                    adjust_proportions();
                }

                std::int32_t scroll_offset() const
                {
                    return (slider_->drag_offset().y - slider_->min_y_offset()) *
                        (content_height_ - pane_height_) / (slider_->max_y_offset() - slider_->min_y_offset());
                }

            private:
                void adjust_proportions()
                {                    
                    auto slider_height = content_height_ ? (pane_height_ * pane_height_) / content_height_ : 0;
                    slider_height = std::max(slider_height, 16);

                    slider_->set_y_offset_limit(0, pane_height_ - slider_height - 4);
                    slider_->set_size(size().x - 4, slider_height);
                }

                Scroll_bar_slider* slider_;
                std::int32_t pane_height_;
                std::int32_t content_height_;
            };

            class Vertical_scroll_bar
                : public Element
            {
            public:
                Vertical_scroll_bar(Vector2i size, std::int32_t content_height)
                {
                    scroll_track_ = create_child<Vertical_scroll_track>(size, content_height);
                    scroll_track_->set_position(2, 2);

                    set_size(size);

                    create_decorator<Background_color>(sf::Color(255, 255, 255, 20));
                }

                void adjust_to_content_size(Vector2i content_size)
                {
                    scroll_track_->set_content_height(content_size.y);
                }

                void adjust_to_size(Vector2i size)
                {
                    auto my_size = Element::size();
                    my_size.y = size.y;

                    set_size(my_size);
                    scroll_track_->set_pane_height(size.y);
                }

                std::int32_t scroll_offset() const
                {
                    return scroll_track_->scroll_offset();
                }

            private:
                Vertical_scroll_track* scroll_track_;
            };

            class Scroll_pane
                : public Element
            {
            public:
                Scroll_pane(Vector2i size)
                    : vertical_scrollbar_(Vector2i(16, size.y), 0)
                {
                    set_size(size);
                    enable_clipping(true);

                    vertical_scrollbar_.set_visible(false);
                    vertical_scrollbar_.set_position(size.x, 0);
                }

                virtual void draw(graphics::Render_target& render_target, graphics::Render_states render_states) const override
                {
                    if (vertical_scrollbar_.visible())
                    {
                        auto elem_offset = offset();
                        render_states.transform.translate(-elem_offset.x, -elem_offset.y);

                        auto view = render_target.getView();
                        render_target.setView(render_target.getDefaultView());

                        vertical_scrollbar_.render(render_target, render_states);

                        render_target.setView(view);
                    }
                }

                virtual void on_resize(Vector2i old_size, Vector2i new_size) override
                {
                    vertical_scrollbar_.set_visible(content_size().y > new_size.y);
                    vertical_scrollbar_.adjust_to_size(new_size);
                }

                virtual void on_content_resize(Vector2i old_size, Vector2i new_size) override
                {
                    vertical_scrollbar_.set_visible(new_size.y > size().y);
                    vertical_scrollbar_.adjust_to_content_size(new_size);
                }

                virtual void on_update(mouse::State mouse_state) override
                {
                    vertical_scrollbar_.update(mouse_state);

                    set_offset(0, -vertical_scrollbar_.scroll_offset());
                }

            private:
                Vertical_scroll_bar vertical_scrollbar_;
            };
        }
    }
}

#endif