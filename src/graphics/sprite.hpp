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

#ifndef GRAPHICS_SPRITE_HPP
#define GRAPHICS_SPRITE_HPP

#include <SFML/Graphics.hpp>

#include "graphics/texture.hpp"

namespace ts
{
    namespace graphics
    {
        class Sprite
            : public sf::Drawable, public sf::Transformable
        {
        public:
            Sprite(const Texture& texture, Rect<int> texture_area = {});

            virtual void draw(sf::RenderTarget& render_target, sf::RenderStates render_states) const override;

            void set_texture_area(Int_rect rect);
            const Int_rect& texture_area() const;

        private:
            mutable sf::Sprite sprite_;
            Rect<int> texture_area_;
            const Texture* texture_;
        };
    }
}

#endif