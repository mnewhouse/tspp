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

#ifndef GAME_ACTION_SCENE_HPP
#define GAME_ACTION_SCENE_HPP

#include "graphics/render_scene.hpp"
#include "controls/control.hpp"

#include "car_image_generator.hpp"
#include "particle_generator.hpp"
#include "track_builder.hpp"
#include "drawable_entity.hpp"
#include "camera.hpp"

namespace ts
{
    namespace world
    {
        class Car;
    }

    namespace game
    {
        class Action_scene
            : public graphics::Render_scene
        {
        public:
            Action_scene(Track_scene_ track_scene);

            virtual void render(sf::RenderTarget& render_target, double frame_time) override;

            void add_car(const world::Car* car, const resources::Player_color& color);
            void remove_car(const world::Car* car);

            void set_followed_entity(const world::Entity* entity, controls::Slot slot = 0);

            void update(std::size_t frame_duration);

            void zoom_in();
            void zoom_out();

        private:
            void reassign_screens();

            void sort_entities_by_level();
            void update_entity_positions();
            void update_component_cache();

            struct Screen
            {
                Screen(Vector2u track_size, Double_rect view_port = { 0.0, 0.0, 1.0, 1.0 });

                Double_rect view_port;
                Camera camera;

                mutable std::vector<Track_component> component_cache;
            };

            void render_screen(Screen& screen, sf::RenderTarget& render_target, double frame_time);
            void render_screen_border(Screen& screen, sf::RenderTarget& render_target, double border_size);

            void calculate_vertex_bounds();

            std::vector<Drawable_entity> drawable_entities_;            
            Track_scene_ track_scene_;
            std::vector<std::vector<Double_rect>> vertex_bounds_;
            
            std::vector<Screen> screens_;
            std::vector<const world::Entity*> followed_entities_;
            bool enable_split_screen_ = true;
            
            Car_image_generator car_image_generator_;
            Particle_generator particle_generator_;
        };
    }
}

#endif