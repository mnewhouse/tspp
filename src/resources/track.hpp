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

#ifndef RESOURCES_TRACK_HPP
#define RESOURCES_TRACK_HPP

#include "control_point.hpp"

namespace ts
{
    namespace resources
    {
        class Track_handle;

        class Terrain_library;
        class Tile_library;

        struct Tile_definition;
        struct Tile_group_definition;

        struct Terrain_definition;
        struct Sub_terrain;

        struct Level_tile;
        struct Placed_tile;

        struct Start_point
        {
            Vector2<double> position;
            Rotation<double> rotation;
            std::size_t level;
        };

        // The Track class provides abstractions for loading and representing
        // a track in memory, keeping track of tile definitions, terrain definitions,
        // placed tiles, control points, and all else that is needed.

        class Track
        {
        public:
            Track();
            explicit Track(const Track_handle& track_handle);
            Track(Track&& other);
            ~Track();

            Track& operator=(Track&& other);

            const Terrain_library& terrain_library() const;
            const Tile_library& tile_library() const;

            void place_tile(const Level_tile& level_tile);

            void define_tile(const Tile_definition& tile_definition);
            void define_tile_group(const Tile_group_definition& tile_group_definition);

            void define_terrain(const Terrain_definition& terrain_definition);
            void define_sub_terrain(const Sub_terrain& sub_terrain);

            void add_control_point(const Control_point& control_point);
            void add_start_point(const Start_point& start_point);

            const std::vector<Level_tile>& tile_list() const;
            const std::vector<Placed_tile>& placed_tiles() const;
            std::size_t placed_tile_count() const;

            const std::vector<Start_point>& start_points() const;
            const std::vector<Control_point>& control_points() const;

            void set_size(Vector2u size, std::size_t num_levels = 1);
            Vector2u size() const;
            std::size_t num_levels() const;

        private:
            void load_from_file(const utf8_string& file_name);

            struct Track_features;
            std::unique_ptr<Track_features> track_features_;
        };
    }
}

#endif