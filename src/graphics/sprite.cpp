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
#include "sprite.hpp"

ts::graphics::Sprite::Sprite(const Texture& texture, Rect<int> texture_area)
: texture_(&texture),
  texture_area_(texture_area)
{
    if (texture_area.width == 0 || texture_area.height == 0)
    {
        texture_area_.width = texture.size().x;
        texture_area_.height = texture.size().y;
    }
}

void ts::graphics::Sprite::draw(sf::RenderTarget& render_target, sf::RenderStates render_states) const
{
    auto draw_sprite = [&, this](const graphics::Sub_texture& sub_texture, const Int_rect& rect)
    {
        sprite_.setTexture(sub_texture.texture);
        sprite_.setTextureRect({ rect.left, rect.top, rect.width, rect.height });

        sprite_.setPosition(getPosition());
        sprite_.setRotation(getRotation());

        auto origin = getOrigin();
        origin.x -= (sub_texture.rect.left + rect.left) - texture_area_.left;
        origin.y -= (sub_texture.rect.top + rect.top) - texture_area_.top;

        sprite_.setOrigin(origin);
        sprite_.setScale(getScale());

        render_target.draw(sprite_, render_states);
    };

    for_each_sub_texture(*texture_, draw_sprite, texture_area_);
}

void ts::graphics::Sprite::set_texture_area(Int_rect rect)
{
    texture_area_ = rect;
}

const ts::Int_rect& ts::graphics::Sprite::texture_area() const
{
    return texture_area_;
}