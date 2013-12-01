/*
 * Turbo Sliders++
 * Copyright (C) 2013 Martin Newhouse
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

#ifndef GRAPHICS_CAMERA_HPP
#define GRAPHICS_CAMERA_HPP

#include <SFML/Graphics/Transformable.hpp>

#include "renderer.hpp"

#include "core/vector2.hpp"
#include "core/rect.hpp"

namespace ts
{
    namespace world
    {
        class Entity;
    }

    namespace graphics
    {
        class Camera
            : public sf::Transformable
        {
        public:
            enum class Mode
            {
                Rotational,
                Fixed
            };

            explicit Camera(Vector2u world_size);

            const world::Entity* target() const;
            Mode mode() const;

            void set_mode(Mode mode);
            void set_target(const world::Entity* target);

            void update_position();

            void update_view(graphics::Render_target& render_target, double frame_time);

        private:
            void clamp_view(sf::View& view);
            void update_transform(double frame_time);

            const world::Entity* camera_target_;
            Vector2d last_position_;

            Vector2d world_size_;
            Mode mode_;
        };
    }
}

#endif