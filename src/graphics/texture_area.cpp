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
#include "texture_area.hpp"

ts::graphics::Texture_area::Texture_area(const Texture& texture, Vector2i area_size, Int_rect texture_rect)
: texture_(&texture),
  area_size_(area_size),
  texture_rect_(texture_rect)
{
}

void ts::graphics::Texture_area::draw(sf::RenderTarget& render_target, sf::RenderStates render_states) const
{
    auto draw_texture = [&](const graphics::Sub_texture& sub_texture, const Int_rect& sub_rect)
    {
        vertex_buffer_.clear();

        for (auto x = sub_texture.rect.left + sub_rect.left - texture_rect_.left; x < area_size_.x; x += texture_rect_.width)
        {
            for (auto y = sub_texture.rect.top + sub_rect.top - texture_rect_.top; y < area_size_.y; y += texture_rect_.height)
            {
                auto old_size = vertex_buffer_.size();
                vertex_buffer_.resize(old_size + 4);

                auto vertex_it = vertex_buffer_.begin() + old_size;
                vertex_it->position.x = static_cast<float>(x);
                vertex_it->position.y = static_cast<float>(y);
                vertex_it->texCoords.x = static_cast<float>(sub_rect.left);
                vertex_it->texCoords.y = static_cast<float>(sub_rect.top);

                auto width = std::min(sub_rect.width, area_size_.x - x);
                auto height = std::min(sub_rect.height, area_size_.y - y);

                auto next = std::next(vertex_it);
                *next = *vertex_it;
                next->position.y += height;
                next->texCoords.y += height;

                ++next, ++vertex_it;
                *next = *vertex_it;
                next->position.x += width;
                next->texCoords.x += width;

                ++next, ++vertex_it;
                *next = *vertex_it;
                next->position.y -= height;
                next->texCoords.y -= height;
            }
        }

        auto states = render_states;
        states.transform *= getTransform();
        states.texture = &sub_texture.texture;

        render_target.draw(vertex_buffer_.data(), vertex_buffer_.size(), sf::Quads, states);
    };

    for_each_sub_texture(*texture_, draw_texture, texture_rect_);
}