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
#include "texture.hpp"

ts::graphics::Texture::Texture(const sf::Image& image, Rect<int> rect)
{
    load_from_image(image, rect);
}

ts::graphics::Texture::Texture(const std::string& file_name, Rect<int> rect)
{
    load_from_file(file_name, rect);
}

ts::graphics::Texture::Texture(Texture&& texture)
    : sub_textures_(std::move(texture.sub_textures_)),
      total_size_(texture.total_size_)
{
    texture.total_size_.x = 0;
    texture.total_size_.y = 0;
}

ts::Vector2u ts::graphics::Texture::size() const
{
    return Vector2u(total_size_.x, total_size_.y);
}

ts::graphics::Texture& ts::graphics::Texture::operator=(Texture&& texture)
{
    sub_textures_ = std::move(texture.sub_textures_);
    total_size_ = texture.total_size_;

    texture.total_size_.x = 0;
    texture.total_size_.y = 0;

    return *this;
}

const std::vector<ts::graphics::Sub_texture>& ts::graphics::Texture::sub_textures() const
{
    return sub_textures_;
}

ts::Int_rect ts::graphics::Texture::sub_texture_rect(std::size_t id) const
{
    auto max_texture_size = sf::Texture::getMaximumSize();
    max_texture_size = 512;

    std::size_t x_count = total_size_.x ? ((total_size_.x - 1) / max_texture_size + 1) : 0;

    Int_rect result;
    result.left = static_cast<std::int32_t>((id % x_count) * max_texture_size);
    result.width = std::min(max_texture_size, total_size_.x - result.left);

    result.top = static_cast<std::int32_t>((id / x_count) * max_texture_size);
    result.height = std::min(max_texture_size, total_size_.y - result.top);

    return result;
}


void ts::graphics::Texture::load_from_file(const std::string& file_name, Rect<int> rect)
{
    sf::Image image;
    if (!image.loadFromFile(file_name)) {
        throw std::runtime_error("could not open image file \"" + file_name + "\"");
    }
            
    load_from_image(image);
}

void ts::graphics::Texture::load_from_image(const sf::Image& image, Int_rect rect)
{
    auto image_size = image.getSize();
    if (rect.width == 0 || rect.height == 0) 
    {
        rect = Int_rect(0, 0, image_size.x, image_size.y);
    }

    auto max_texture_size = sf::Texture::getMaximumSize();
    max_texture_size = 512;

    // Get the number of sub-textures
    auto x_count = rect.width ? (rect.width - 1) / max_texture_size + 1 : 0;
    auto y_count = rect.height ? (rect.height - 1) / max_texture_size + 1 : 0;
    auto sub_texture_count = x_count * y_count;

    sub_textures_.resize(sub_texture_count);
    total_size_ = image_size;

    for (std::size_t id = 0; id != sub_texture_count; ++id) {
        auto& sub_texture = sub_textures_[id];
        auto& texture = sub_texture.texture;
        auto& sub_rect = sub_texture.rect;

        sub_rect = intersection(sub_texture_rect(id), rect);        

        sf::IntRect sf_rect(sub_rect.left, sub_rect.top, sub_rect.width, sub_rect.height);
        if (!texture.loadFromImage(image, sf_rect)) {
            throw std::runtime_error("could not create texture");
        }
    }
}