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

#include "stdinc.hpp"
#include "window_template.hpp"

#include "user_interface/gui_element.hpp"


namespace
{
    ts::gui::Tiled_background::Texture_attributes load_attributes()
    {
        ts::gui::Tiled_background::Texture_attributes attributes;
        attributes.left_offset = 0;
        attributes.top_offset = 0;
        attributes.left_width = 40;
        attributes.top_height = 40;
        attributes.right_width = 40;
        attributes.bottom_height = 40;
        attributes.center_width = 513;
        attributes.middle_height = 457;

        return attributes;
    }
}

namespace ts
{
    namespace gui_definitions
    {
        static const ts::utf8_string image_path = "data/window.png";
        static const ts::gui::Tiled_background::Texture_attributes attributes = load_attributes();
    }
}


ts::gui::Element* ts::gui_definitions::create_styled_window(gui::Context* context, gui::Element* parent, Vector2i size)
{
    auto background_texture = context->load_texture(image_path);
    auto window_style = gui::make_background_style<gui::Tiled_background>(background_texture, attributes);

    auto window = parent->create_child<gui::Element>(size);
    window->set_background_style(window_style);
    return window;
}