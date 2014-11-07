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

#ifndef GAME_CAMERA_HPP
#define GAME_CAMERA_HPP

#include <SFML/Graphics.hpp>

namespace ts
{
    namespace world
    {
        class Entity;
    }

    namespace game
    {
        class Camera
        {
        public:
            enum class Mode
            {
                Rotational,
                Fixed
            };

            explicit Camera(Vector2u screen_size, Vector2u world_size);

            const world::Entity* target() const;
            Mode mode() const;

            void set_mode(Mode mode);
            void set_target(const world::Entity* target);
            void set_position(Vector2<double> position);
            void set_rotation(Rotation<double> rotation);

            const sf::View& view() const;
            void update_view(Double_rect view_port, double frame_time);

            void update_position();

            void set_zoom_level(double zoom_level);

            Vector2<double> position() const;
            Rotation<double> rotation() const;
            double zoom_level() const;

            bool is_area_visible(const Double_rect& rect) const;
            double fit_in_screen_zoom_level() const;

        private:            
            void clamp_view();
            void update_visible_area();

            Mode mode_;
            Double_rect visible_area_;
            Vector2<double> screen_size_;

            const world::Entity* camera_target_;
            Vector2<double> last_position_;
            Vector2<double> world_size_;
            Vector2<double> position_;
            Rotation<double> rotation_;

            double zoom_level_;
            sf::View view_;                      
        };
    }
}

#endif