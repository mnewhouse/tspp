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

#ifndef SCENE_DRAWABLE_ENTITY_HPP
#define SCENE_DRAWABLE_ENTITY_HPP

#include "graphics/texture.hpp"

#include "resources/image_type.hpp"

namespace ts
{
    namespace world
    {
        class Entity;
    }

    namespace scene
    {
        // Drawable_entity
        // Represents an entity that can be drawn on the screen using a texture.
        // It supports interpolation between the entity's current and previous position,
        // allowing for a smooth visual experience.

        class Drawable_entity
        {
        public:
            Drawable_entity(const world::Entity* entity, std::shared_ptr<graphics::Texture> texture, resources::Image_type image_type);
            
            void draw(sf::RenderTarget& render_target, sf::RenderStates states, double frame_time) const;

            const world::Entity* entity() const;
            void set_scale(Vector2<double> scale);

            void set_texture_rect(Int_rect texture_rect);

            void update_position();
            
        private:
            std::shared_ptr<graphics::Texture> texture_;
            const world::Entity* entity_;

            resources::Image_type image_type_;
            Int_rect texture_rect_;
            Vector2<double> scale_;

            Vector2<double> last_position_;            
        };
    }
}

#endif