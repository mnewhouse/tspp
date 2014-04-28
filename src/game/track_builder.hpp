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

#ifndef GAME_TRACK_BUILDER_HPP
#define GAME_TRACK_BUILDER_HPP

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "core/rect.hpp"

#include <vector>
#include <set>
#include <memory>

namespace ts
{
    namespace resources
    {
        class Track;
    }

    namespace graphics
    {
        class Texture;
    }

    namespace game
    {
        struct Track_component
        {
            Track_component()
            : vertices(), texture(nullptr), level(0)
            {
            }

            Track_component(std::vector<sf::Vertex> vertices, const sf::Texture* texture, int level)
                : vertices(std::move(vertices)), texture(texture), level(level)
            {
            }

            std::vector<sf::Vertex> vertices;
            const sf::Texture* texture;
            Rect<float> bounding_box;
            int level;
        };

        struct Track_builder_result
        {
            std::vector<Track_component> components;
            std::set<std::shared_ptr<graphics::Texture>> textures;
        };

        Track_builder_result build_track(const resources::Track& track);

    }

}

#endif