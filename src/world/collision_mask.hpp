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

#ifndef WORLD_COLLISION_MASK_HPP
#define WORLD_COLLISION_MASK_HPP

#include <vector>

#include "core/vector2.hpp"
#include "core/rect.hpp"

namespace ts
{
    namespace resources
    {
        class Pattern;
    }

    namespace world
    {
        class Collision_mask
        {
        public:
            Collision_mask(const resources::Pattern& pattern, Int_rect rect);

            const std::vector<Vector2i>& outline() const;
            const Int_rect& bounding_box() const;

        private:
            std::vector<Vector2i> outline_;
            Int_rect bounding_box_;
        };
    }
}

#endif