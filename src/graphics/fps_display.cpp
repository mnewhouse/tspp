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

#include "fps_display.hpp"

#include "fonts/sans.hpp"
#include "fonts/font.hpp"

void ts::graphics::FPS_Display::render(Render_target& render_target)
{
    ++fps_counter_;

    std::string fps("FPS: ");
    fps += std::to_string(static_cast<int>(fps_counter_.current_fps()));

    sf::Text fps_display(fps, fonts::get_font_by_type<fonts::Sans>(), 18);

    auto screen_size = render_target.getSize();
    fps_display.setPosition(screen_size.x - 120.f, 20.f);
    fps_display.setColor(sf::Color(255, 255, 0));

    render_target.draw(fps_display);
}