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

#ifndef GUI_TEXT_ELEMENT_HPP
#define GUI_TEXT_ELEMENT_HPP

#include "../stylable_element.hpp"

#include "graphics/text.hpp"

namespace ts
{
    namespace gui
    {
        using Font_face = graphics::Font_face;

        struct Text_style
        {
            std::uint32_t character_size;
            sf::Color color;
            Font_face font;
        };

        class Text_element
            : public Stylable_element<Text_style>
        {
        public:
            Text_element(graphics::Composite_text text, const Text_style& style);
            Text_element(graphics::Composite_text text, const Styler_type& styler);

            Text_element(utf8_string text, const Text_style& style);
            Text_element(utf8_string text, const Styler_type& styler);

            virtual void draw(sf::RenderTarget& render_target, sf::RenderStates render_states) const override;
            virtual Vector2<double> content_size() const override;

            void set_text(utf8_string text);
            const utf8_string& text() const;
            const graphics::Text& text_item() const;

            void center_text_horizontal(bool center = true);
            void center_text_vertical(bool center = true);

        private:
            void apply_centering();

            virtual void apply_style(const Text_style& style) override;
            void apply_style_impl(const Text_style& style);

            virtual void on_layout() override;

            graphics::Text text_item_;

            bool center_horizontal_ = false;
            bool center_vertical_ = false;

            Vector2<double> offset_;
        };
    }
}

#endif

