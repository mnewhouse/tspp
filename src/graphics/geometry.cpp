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
#include "geometry.hpp"

void ts::graphics::Geometry::draw(sf::RenderTarget& render_target, sf::RenderStates render_states) const
{
    render_states.texture = texture;
    render_states.transform.translate(static_cast<float>(position.x), static_cast<float>(position.y));

    render_target.draw(vertices.data(), static_cast<unsigned int>(vertices.size()), primitive_type, render_states);
}