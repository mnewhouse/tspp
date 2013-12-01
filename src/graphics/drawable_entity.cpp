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

#include "drawable_entity.hpp"
#include "core/rotation.hpp"

#include "world/entity.hpp"
#include "texture.hpp"

#include <iostream>

ts::graphics::Drawable_entity::Drawable_entity
    (const world::Entity* entity, Handle<graphics::Texture> texture, resources::Image_type image_type)
    : texture_(std::move(texture)), 
      entity_(entity), 
      image_type_(image_type),
      scale_(1.0, 1.0),
      last_position_(entity->position())
{
}

void ts::graphics::Drawable_entity::draw(graphics::Render_target& render_target, graphics::Render_states states, double frame_time) const
{
    const auto rotation = entity_->rotation();

    const auto position = last_position_ + ((entity_->position() - last_position_) * frame_time);

    sf::Sprite sprite;
    sprite.setPosition(float(position.x), float(position.y));
    sprite.setScale(float(scale_.x), float(scale_.y));
    
    auto draw_sprite = [&](const sf::Texture& texture, const sf::IntRect& rect)
    {
        sprite.setTexture(texture);
        sprite.setTextureRect(rect);

        sprite.setOrigin(rect.width * 0.5f, rect.height * 0.5f);

        render_target.draw(sprite, states);
    };

    if (image_type_ == resources::Image_type::Rotational) {
        auto size = texture_->size();

        auto frame_count = size.x / size.y;
        auto frame_width = size.x / frame_count;

        auto frame_id = static_cast<int>((frame_count / 360) * rotation.degrees() - 0.5);
        while (frame_id < 0) frame_id += frame_count;
        frame_id %= frame_count;

        auto discrete_rotation = frame_id * (360 / frame_count);

        sprite.setRotation(float(rotation.degrees() - discrete_rotation));

        Rect<int> rect(frame_id * frame_width, 0, frame_width, size.y);

        texture_->perform(draw_sprite, rect);
    }

    else {
        sprite.setRotation(float(rotation.degrees()));
    }
}

void ts::graphics::Drawable_entity::set_scale(Vector2d scale)
{
    scale_.x = 1.0 / scale.x;
    scale_.y = 1.0 / scale.y;
}

void ts::graphics::Drawable_entity::update_position()
{
    last_position_ = entity_->position();
}

const ts::world::Entity* ts::graphics::Drawable_entity::entity() const
{
    return entity_;
}