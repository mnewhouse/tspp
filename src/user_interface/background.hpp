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

#ifndef GUI_BACKGROUND_HPP
#define GUI_BACKGROUND_HPP

#include <SFML/Graphics.hpp>

#include "graphics/texture.hpp"

namespace ts
{
    namespace gui
    {
        class Element;

        struct Background_base
        {
            virtual void draw(const Element& element, sf::RenderTarget& render_target, sf::RenderStates render_states) const = 0;
        };

        struct Plain_background
            : public Background_base
        {
            Plain_background(sf::Color color);

            virtual void draw(const Element& element, sf::RenderTarget& render_target, sf::RenderStates render_states) const override;

            void set_color(sf::Color color);

        private:
            sf::Color color_;
        };

        struct Textured_background
            : public Background_base
        {
            enum class Mode
            {
                Default,
                Stretched,
                Centered
            };

            Textured_background(std::shared_ptr<graphics::Texture> texture, Rect<int> texture_area = {}, Mode mode = Mode::Stretched);

            virtual void draw(const Element& element, sf::RenderTarget& render_target, sf::RenderStates render_states) const override;

            void set_scale(Vector2<double> scale);
            void set_sub_rect(Int_rect rect);

        private:
            std::shared_ptr<graphics::Texture> texture_;
            Int_rect texture_area_;
            Mode mode_;           

            Vector2<double> scale_;
        };

        struct Tiled_background
            : public Background_base
        {            
        public:
            struct Texture_attributes
            {
                int left_offset;
                int top_offset;
                int left_width;
                int center_width;
                int right_width;
                int top_height;
                int middle_height;
                int bottom_height;
            };

            Tiled_background(std::shared_ptr<graphics::Texture> texture, Texture_attributes dimensions);

            virtual void draw(const Element& element, sf::RenderTarget& render_target, sf::RenderStates render_states) const override;

        private:
            std::shared_ptr<graphics::Texture> texture_;
            Texture_attributes attributes_;
        };

        using Background = std::shared_ptr<Background_base>;
    }
}

#endif