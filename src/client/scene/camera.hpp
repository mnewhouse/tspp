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

#ifndef SCENE_CAMERA_HPP
#define SCENE_CAMERA_HPP

#include <SFML/Graphics.hpp>

namespace ts
{
    namespace world
    {
        class Entity;
    }

    namespace scene
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

            void set_target(const world::Entity* target, bool follow = true);
            const world::Entity* target() const;

           

            Mode mode() const;
            void set_mode(Mode mode);
            
            void follow_target(bool follow);
            bool is_target_followed() const;

            void constrict_to_screen(bool constrict);
            bool is_constricted_to_screen() const;

            void set_position(Vector2<double> position);
            void set_rotation(Rotation<double> rotation);
            void set_zoom_level(double zoom_level);

            Vector2<double> position() const;
            Rotation<double> rotation() const;
            double zoom_level() const;

            const sf::View& view() const;
            void update_view(Double_rect view_port, double frame_time);

            void update_position();                    

            bool is_area_visible(const Double_rect& rect) const;
            double fit_in_screen_zoom_level() const;

        private:            
            void constrict_view();
            void update_visible_area();

            Mode mode_;
            Double_rect visible_area_;
            Vector2<double> screen_size_;

            const world::Entity* camera_target_;
            bool follow_target_ = false;
            bool constrict_to_screen_ = false;

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