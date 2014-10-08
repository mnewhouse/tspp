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

#include "resources/image_type.hpp"

#include "track_builder.hpp"
#include "view_context.hpp"

namespace ts
{
    namespace resources
    {
        class Video_settings;
        struct Player_color;
    }

    namespace world
    {
        class Car;
    }

    namespace game
    {
        class Car_image_generator;
        class Particle_generator;
        class Drawable_entity;

        struct Drawable_entity_definition
        {
            explicit Drawable_entity_definition(std::shared_ptr<graphics::Texture> texture);

            std::shared_ptr<graphics::Texture> texture;
            Int_rect texture_rect;
            Vector2<double> scale;
            resources::Image_type image_type;
        };

        class Action_scene
            : public graphics::Render_scene
        {
        public:
            Action_scene(Track_scene track_scene, const resources::Video_settings& video_settings);
            virtual ~Action_scene();

            virtual void render(sf::RenderTarget& render_target, double frame_time) override;

            void add_car(const world::Car* car, const Drawable_entity_definition& drawable_definition);
            void remove_car(const world::Car* car);

            void set_followed_entity(const world::Entity* entity);
            void add_followed_entity(const world::Entity* entity);

            View_context* view_context();


            // Why two public update functions you ask?
            // The 'update' function should be called *after* the world update.
            void update(std::size_t frame_duration);

            // The 'update_entities' function on the other hand should be called *before* the world update,
            // to allow for interpolating the positions towards the current world state.
            void update_entities(std::size_t frame_duration);

        private:
            void reassign_screens();

            void sort_entities_by_level();
            void update_entity_positions();
            void update_component_cache();
            void update_cameras(double frame_time);            

            void render_view(std::size_t view_index, sf::RenderTarget& render_target, double frame_time);
            void render_view_border(const View& view, sf::RenderTarget& render_target, double border_size);

            void calculate_vertex_bounds();

            std::vector<Drawable_entity> drawable_entities_;            
            Track_scene track_scene_;
            std::vector<std::vector<Double_rect>> vertex_bounds_;
            std::vector<std::vector<Track_component>> component_cache_;
            
            View_context view_context_;
            std::vector<const world::Entity*> followed_entities_;

            bool enable_split_screen_ = true;
            bool dirty_component_cache_ = true;
            
            std::unique_ptr<Car_image_generator> car_image_generator_;
            std::unique_ptr<Particle_generator> particle_generator_;
        };
    }
}

#endif