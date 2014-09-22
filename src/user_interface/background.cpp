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
#include "background.hpp"
#include "gui_element.hpp"

#include "graphics/sprite.hpp"
#include "graphics/texture_area.hpp"

ts::gui::Plain_background::Plain_background(sf::Color color)
: color_(color)
{
}

void ts::gui::Plain_background::set_color(sf::Color color)
{
    color_ = color;
}

void ts::gui::Plain_background::draw(const Element& element, sf::RenderTarget& render_target, sf::RenderStates render_states) const
{
    auto elem_size = element.size();
    float left = 0.0f;
    float top = 0.0f;
    float right = static_cast<float>(elem_size.x);
    float bottom = static_cast<float>(elem_size.y);

    sf::Vertex vertices[4];
    vertices[0].position = { left, top };
    vertices[1].position = { left, bottom };
    vertices[2].position = { right, bottom };
    vertices[3].position = { right, top };
    
    for (auto& vertex : vertices) vertex.color = color_;

    render_target.draw(vertices, array_size(vertices), sf::Quads, render_states);
}

ts::gui::Textured_background::Textured_background(std::shared_ptr<graphics::Texture> texture, Rect<int> texture_area, Mode mode)
: texture_(std::move(texture)),
  texture_area_(texture_area),
  mode_(mode)
{
    if (texture_area.width == 0 || texture_area.height == 0)
    {
        auto texture_size = texture_->size();
        texture_area_.width = texture_size.x;
        texture_area_.height = texture_size.y;
    }
}

ts::gui::Tiled_background::Tiled_background(std::shared_ptr<graphics::Texture> texture, Texture_attributes attributes)
: texture_(std::move(texture)),
  attributes_(attributes)
{
}

void ts::gui::Tiled_background::draw(const Element& element, sf::RenderTarget& render_target, sf::RenderStates render_states) const
{
    Int_rect rect(attributes_.left_offset, attributes_.top_offset, attributes_.left_width, attributes_.top_height);
    auto elem_size = element.size();

    auto center_width = static_cast<float>(elem_size.x) - attributes_.left_width - attributes_.right_width;
    auto middle_height = static_cast<float>(elem_size.y) - attributes_.top_height - attributes_.bottom_height;

    sf::Vector2f position = {};

    {
        graphics::Sprite sprite(*texture_, rect);
        sprite.setPosition(position);

        render_target.draw(sprite, render_states);

        rect.top += attributes_.top_height;
        position.y += attributes_.top_height;
    }

    if (middle_height > 0.0)
    {
        Vector2i area_size(attributes_.left_width, static_cast<std::int32_t>(middle_height));
        graphics::Texture_area area(*texture_, area_size, rect);
        area.setPosition(position);

        render_target.draw(area, render_states);
    }

    rect.top += attributes_.middle_height;
    position.y += middle_height;

    {
        graphics::Sprite sprite(*texture_, rect);
        sprite.setPosition(position);
        render_target.draw(sprite, render_states);

        rect.left += attributes_.left_width;
        position.x += attributes_.left_width;
    }

    {
        if (center_width > 0.0)
        {
            Vector2i area_size(static_cast<std::int32_t>(center_width), attributes_.bottom_height);
            graphics::Texture_area area(*texture_, area_size, rect);
            area.setPosition(position);

            render_target.draw(area, render_states);
        }

        rect.top -= attributes_.middle_height;
        position.y -= middle_height;
    }

    if (center_width > 0.0 && middle_height > 0.0)
    {
        Vector2i area_size(static_cast<std::int32_t>(center_width), static_cast<std::int32_t>(middle_height));
        graphics::Texture_area area(*texture_, area_size, rect);
        area.setPosition(position);

        render_target.draw(area, render_states);
    }

    rect.top -= attributes_.top_height;
    position.y -= attributes_.top_height;

    if (center_width > 0.0)
    {
        Vector2i area_size(static_cast<std::int32_t>(center_width), attributes_.top_height);
        graphics::Texture_area area(*texture_, area_size, rect);
        area.setPosition(position);

        render_target.draw(area, render_states);
    }

    rect.left += attributes_.center_width;
    position.x += center_width;

    {
        graphics::Sprite sprite(*texture_, rect);
        sprite.setPosition(position);

        render_target.draw(sprite, render_states);

        rect.top += attributes_.top_height;
        position.y += attributes_.top_height;
    }

    if (middle_height > 0.0)
    {
        Vector2i area_size(attributes_.right_width, static_cast<std::int32_t>(middle_height));
        graphics::Texture_area area(*texture_, area_size, rect);
        area.setPosition(position);
        
        render_target.draw(area, render_states);
    }

    rect.top += attributes_.middle_height;
    position.y += middle_height;

    {
        graphics::Sprite sprite(*texture_, rect);
        sprite.setPosition(position);
        render_target.draw(sprite, render_states);
    }
}

void ts::gui::Textured_background::draw(const Element& element, sf::RenderTarget& render_target, sf::RenderStates render_states) const
{
    Vector2f elem_size = element.size();
    Rect<double> dest_area(0.0, 0.0, elem_size.x, elem_size.y);

    sf::Vector2f scale(static_cast<float>(scale_.x), static_cast<float>(scale_.y));
    sf::Vector2f position(0.0f, 0.0f);

    if (mode_ == Mode::Centered)
    {
        auto width = texture_area_.width * scale.x;
        auto height = texture_area_.height * scale.y;

        position.x = (elem_size.x - width) * 0.5f;
        position.y = (elem_size.y - height) * 0.5f;
    }

    else if (mode_ == Mode::Stretched)
    {
        scale.x = elem_size.x / texture_area_.width;
        scale.y = elem_size.y / texture_area_.height;
    }


    graphics::Sprite sprite(*texture_, texture_area_);

    sprite.setScale(scale);
    sprite.setPosition(position);

    render_target.draw(sprite, render_states);
}

void ts::gui::Textured_background::set_scale(Vector2<double> scale)
{
    scale_ = scale;
}

void ts::gui::Textured_background::set_sub_rect(Int_rect rect)
{
    texture_area_ = rect;
}