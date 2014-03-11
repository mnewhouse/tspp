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

#ifndef RESOURCES_COLLISION_MASK_HPP
#define RESOURCES_COLLISION_MASK_HPP

#include <vector>
#include <memory>
#include <cstdint>

#include "core/vector2.hpp"
#include "core/rotation.hpp"

namespace ts
{
    namespace resources
    {
        class Pattern;
        class Terrain_library;
    }

    namespace world
    {
        class Collision_bitmap
        {
        public:
            Collision_bitmap(const std::shared_ptr<resources::Pattern>& pattern);

            void set_rotation(Rotation<double> rotation);
            void set_position(Vector2i position);

            const Vector2u& size() const;
            const std::vector<std::uint64_t>& bitmap() const;
            Rotation<double> rotation() const;

        private:
            void update(Rotation<double> rotation);
            
            Vector2u bitmap_size_;
            std::vector<std::uint64_t> bitmap_;
            Rotation<double> rotation_;

            std::shared_ptr<resources::Pattern> pattern_;
        };

        class Static_collision_bitmap
        {
        public:
            Static_collision_bitmap(const resources::Pattern& pattern, const resources::Terrain_library& terrain_lib, std::size_t num_levels);

            const Vector2u& size() const;
            const std::vector<std::uint64_t>& bitmap() const;
            std::size_t num_levels() const;

            bool operator()(Vector2u point, std::size_t level = 0) const;

        private:
            std::vector<std::uint64_t> bitmap_;
            Vector2u bitmap_size_;
            std::size_t num_levels_;
        };

        struct Collision_point
        {
            bool collided = false;
            Vector2i point;

            operator bool() const { return collided; }
        };

        Collision_point collision_test(const Collision_bitmap& subject, Vector2i subject_position,
                                       const Collision_bitmap& object, Vector2i object_position);

        Collision_point collision_test(const Collision_bitmap& subject, const Static_collision_bitmap& scenery,
                                       Vector2i subject_position, std::size_t subject_level);
    }
}



#endif