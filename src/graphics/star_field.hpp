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

#include "render_scene.hpp"

#include <vector>

namespace ts
{

    namespace graphics
    {

        class Star_field
            : public Render_scene
        {
        public:
            explicit Star_field(std::size_t star_count, sf::Color background_color = sf::Color::Transparent);

            virtual void render(Render_target& render_target) override;

        private:
            struct Star_position
            {
                double x;
                double y;
                double z;
            };

            Star_position random_position(bool randomize_z = false) const;

            std::vector<Star_position> stars_;
            std::vector<sf::Vertex> vertices_;

            sf::Clock clock_;

            sf::Color background_color_;

        };

    }

}