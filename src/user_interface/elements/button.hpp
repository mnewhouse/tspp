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

#ifndef GUI_ELEMENTS_BUTTON_HPP
#define GUI_ELEMENTS_BUTTON_HPP

#include "user_interface/context.hpp"

#include "rectangle_region.hpp"
#include "core/handle.hpp"

namespace ts
{
    namespace gui
    {
        namespace elements
        {

            template <typename RegionType>
            class Button
            {
            public:
                Button(element_id id, RegionType region)
                    : id_(id), region_(region)
                {}

                Element_state operator()(const Handle<Context>& context);

            private:
                element_id id_;
                RegionType region_;
            };

            template <typename T>
            using Rectangular_button = Button<Rectangle_region<T>>;

            template <typename RegionType>
            Element_state Button<RegionType>::operator()(const Handle<Context>& context)
            {
                const auto& mouse_state = context->mouse_state();
                bool button_state = (mouse_state.button_state & mouse::Button::Left) != 0;

                if (region_(mouse_state.position)) {
                    context->set_hot_item(id_);

                    if (button_state) {
                        if (!context->active_item()) {
                            context->set_active_item(id_);
                        }
                    }
                }

                Element_state element_state;
                element_state.hot = (context->hot_item() == id_);
                element_state.active = (context->active_item() == id_);
                element_state.clicked = element_state.active && element_state.hot && !button_state;

                return element_state;
            }
        }
    }
}

#endif