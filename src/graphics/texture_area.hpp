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

#ifndef GRAPHICS_TEXTURE_AREA_HPP
#define GRAPHICS_TEXTURE_AREA_HPP

#include <SFML/Graphics.hpp>

#include "graphics/texture.hpp"

namespace ts
{
    namespace graphics
    {
        class Texture_area
            : public sf::Drawable, public sf::Transformable
        {
        public:
            Texture_area(const Texture& texture, Vector2i area_size, Int_rect texture_rect = {});

            virtual void draw(sf::RenderTarget& render_target, sf::RenderStates render_states) const override;

        private:
            const Texture* texture_;
            Rect<int> texture_rect_;
            Vector2i area_size_;

            mutable std::vector<sf::Vertex> vertex_buffer_;
        };
    }
}

#endif