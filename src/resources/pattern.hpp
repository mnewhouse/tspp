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

#ifndef RESOURCES_PATTERN_FILE_HPP
#define RESOURCES_PATTERN_FILE_HPP

#include "terrain_definition.hpp"

namespace ts
{
    namespace resources
    {
        class Pattern
        {
        public:
            explicit Pattern(Vector2u size = Vector2u{});
            explicit Pattern(const utf8_string& file_name, Int_rect rect = Int_rect{});            

            Pattern(Pattern&& pattern);
            Pattern(const Pattern& pattern) = default;

            Pattern& operator=(Pattern&& pattern);
            Pattern& operator=(const Pattern& pattern) = default;

            void load_from_file(const utf8_string& file_name, Int_rect rect);

            const Terrain_id& operator()(std::size_t x, std::size_t y) const;
            Terrain_id& operator()(std::uint32_t x, std::uint32_t y);

            Vector2u size() const;
            void resize(Vector2u new_size);
            void resize(std::uint32_t x, std::uint32_t y);

            using iterator = Terrain_id*;
            iterator begin();
            iterator end();
            iterator row_begin(std::uint32_t row_id);
            iterator row_end(std::uint32_t row_id);

            using const_iterator = const Terrain_id*;
            const_iterator begin() const;
            const_iterator end() const;
            const_iterator row_begin(std::uint32_t row_id) const;
            const_iterator row_end(std::uint32_t row_id) const;

        private:
            Vector2u size_;
            std::vector<Terrain_id> bytes_;
        };
    }
}


#endif