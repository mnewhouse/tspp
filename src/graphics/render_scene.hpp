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

#ifndef GRAPHICS_RENDER_SCENE_HPP
#define GRAPHICS_RENDER_SCENE_HPP

#include <SFML/Graphics.hpp>

namespace ts
{
    namespace graphics
    {

        class Render_scene
        {
        public:
            Render_scene() : visible_(true) {}

            void render_if_visible(sf::RenderTarget& render_target, double frame_time) { if (visible()) render(render_target, frame_time); }

            void set_visible(bool visible) { visible_ = visible; }
            bool visible() const { return visible_; }

        private:
            virtual void render(sf::RenderTarget& render_target, double frame_time) = 0;

            bool visible_;
        };

        /*
        struct Background
            : public Render_scene
        {
        public:
            template <typename ConcreteType>
            Background(const std::shared_ptr<ConcreteType>& scene)
                : scene_(std::static_pointer_cast<Render_scene>(scene))
            {}

            Background() {}

            virtual void render(sf::RenderTarget& render_target) override
            {
                scene_->render_if_visible(render_target);
            }

            operator bool() const
            {
                return scene_.operator bool();
            }

        private:
            std::shared_ptr<Render_scene> scene_;
        };
        */

    }

}

#endif