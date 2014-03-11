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

#ifndef RESOURCES_TRACK_HPP
#define RESOURCES_TRACK_HPP

#include "tile_library.hpp"
#include "terrain_library.hpp"

#include <istream>

namespace ts
{
    namespace resources
    {
        class Track_handle;

        // The Track class provides abstractions for loading and representing
        // a track in memory, keeping track of tile definitions, terrain definitions,
        // placed tiles, control points, and all else that is needed.

        class Track
        {
        public:
            Track();
            explicit Track(const Track_handle& track_handle);

            void load_from_file(const std::string& file_name);

            const Terrain_library& terrain_library() const;
            const Tile_library& tile_library() const;

            const std::vector<Level_tile>& tile_list() const;
            Vector2u size() const;

            std::size_t num_levels() const;

            std::string find_include_path(const std::string& file_name) const;

        private:
            void include(const std::string& file_name);
            void include(const std::string& file_name, std::size_t recursion_depth);

            void include(std::istream& stream, std::size_t recursion_depth = 0);

            void add_tile(Level_tile level_tile);
            void parse_tile_definition(std::istream& stream, const std::string& pattern_name, const std::string& image_name);
            void parse_tile_group_definition(std::istream& stream, Tile_id id, std::size_t size);

            void parse_terrain(std::istream& stream, const std::string& terrain_name);
            void parse_control_points(std::istream& stream, std::size_t count);
            void parse_start_points(std::istream& stream, std::size_t count);

            std::vector<Level_tile> tile_list_;
            Vector2u track_size_;
            std::size_t num_levels_;

            Tile_library tile_lib_;
            Terrain_library terrain_lib_;

            std::string track_directory_;
        };
    }
}

#endif