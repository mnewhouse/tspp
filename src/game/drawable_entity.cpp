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
#include "drawable_entity.hpp"
#include "world/entity.hpp"

#include "graphics/sprite.hpp"

ts::game::Drawable_entity::Drawable_entity
    (const world::Entity* entity, std::shared_ptr<graphics::Texture> texture, resources::Image_type image_type)
    : texture_(std::move(texture)), 
      entity_(entity), 
      image_type_(image_type),
      scale_(1.0, 1.0),
      last_position_(entity->position())
{
}

void ts::game::Drawable_entity::draw(sf::RenderTarget& render_target, sf::RenderStates states, double frame_time) const
{
    const auto rotation = entity_->rotation();
    const auto new_position = entity_->position();
    const auto position = interpolate_position(last_position_, new_position, frame_time);

    if (image_type_ == resources::Image_type::Rotational) 
    {
        auto size = texture_->size();

        auto frame_count = size.x / size.y;
        auto frame_width = size.x / frame_count;

        auto frame_id = static_cast<int>(std::round((frame_count / 360.0) * rotation.degrees()));
        while (frame_id < 0) frame_id += frame_count;
        frame_id %= frame_count;

        auto frame_rotation = Rotation<double>::degrees(frame_id * (360.0 / frame_count));
        auto rotation_offset = rotation - frame_rotation;

        Int_rect rect(frame_id * frame_width, 0, frame_width, size.y);

        graphics::Sprite sprite(*texture_, rect);
        sprite.setRotation(float(rotation_offset.degrees()));
        sprite.setPosition(float(position.x), float(position.y));
        sprite.setScale(float(scale_.x), float(scale_.y));
        sprite.setOrigin(rect.width * 0.5f, rect.height * 0.5f);

        render_target.draw(sprite, states);

    }

    else 
    {
        graphics::Sprite sprite(*texture_);
        sprite.setRotation(static_cast<float>(rotation.degrees()));        
        sprite.setPosition(static_cast<float>(position.x), static_cast<float>(position.y));
        sprite.setScale(static_cast<float>(scale_.x), static_cast<float>(scale_.y));
        sprite.setOrigin(texture_->size().x * 0.5f, texture_->size().y * 0.5f);

        render_target.draw(sprite, states);
    }
}

void ts::game::Drawable_entity::set_scale(Vector2d scale)
{
    scale_.x = 1.0 / scale.x;
    scale_.y = 1.0 / scale.y;
}

void ts::game::Drawable_entity::update_position()
{
    last_position_ = entity_->position();
}

const ts::world::Entity* ts::game::Drawable_entity::entity() const
{
    return entity_;
}