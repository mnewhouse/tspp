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

#ifndef RESOURCES_TILE_HPP
#define RESOURCES_TILE_HPP

namespace ts
{

    namespace resources
    {

        using Tile_id = int;

        struct Tile
        {
            Tile()
            : id(), position(), rotation()
            {}

            Tile_id id;
            Vector2<double> position;
            Rotation<double> rotation;
        };

        struct Level_tile
            : public Tile
        {
            Level_tile(Tile placed_tile = Tile())
            : Tile(placed_tile),
              level(0)
            {}

            std::uint32_t level;
        };

        struct Tile_definition
        {
        public:
            Tile_definition(utf8_string pattern_file, utf8_string image_file)
                : id(), image_rect(), pattern_rect(),
                  pattern_file_(std::move(pattern_file)),
                  image_file_(std::move(image_file))
            {}

            Tile_id id;
            Int_rect image_rect;
            Int_rect pattern_rect;

            const utf8_string& image_file() const { return image_file_; }
            const utf8_string& pattern_file() const { return pattern_file_; }

        private:
            utf8_string pattern_file_;
            utf8_string image_file_;
        };

        struct Tile_group_definition
        {
        public:
            Tile_group_definition(Tile_id id, std::size_t size)
                : id_(id)
            {
                sub_tiles_.reserve(size);
            }

            void add_sub_tile(const Level_tile& tile)
            {
                sub_tiles_.push_back(tile);
            }

            Tile_id id() const
            {
                return id_;
            }

            const std::vector<Level_tile>& sub_tiles() const
            {
                return sub_tiles_;
            }

        private:
            Tile_id id_;
            std::vector<Level_tile> sub_tiles_;
        };

        struct Placed_tile
        {
            const Tile_definition* tile_def = nullptr;
            Level_tile tile;
        };

        inline std::istream& operator>>(std::istream& stream, Tile_definition& tile_def)
        {
            return stream >> tile_def.id >> tile_def.pattern_rect >> tile_def.image_rect;
        }

        inline std::istream& operator>>(std::istream& stream, Tile& tile)
        {
            return stream >> tile.id >> tile.position >> tile.rotation;
        }

        inline std::istream& operator>>(std::istream& stream, Level_tile& tile)
        {
            return stream >> tile.level >> static_cast<Tile&>(tile);
        }


    }

}

#endif