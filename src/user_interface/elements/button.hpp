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

#ifndef GUI_ELEMENTS_BUTTON_HPP
#define GUI_ELEMENTS_BUTTON_HPP

#include "../gui_element.hpp"

#include "rectangle_region.hpp"

namespace ts
{
    namespace gui
    {
        namespace elements
        {
            template <typename RegionType>
            struct Button
                : public Element
            {
            public:
                Button(RegionType region)
                    : region_(region)
                {}           

                virtual bool hover(Vector2i mouse_position) const
                {
                    return region_(mouse_position);
                }

            private:
                RegionType region_;
            };

            using Rectangular_button = Button<Rectangle_region>;
        }
    }
}

#endif