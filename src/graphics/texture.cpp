#include "texture.hpp"

#include <iostream>
#include <algorithm>

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

ts::Int_rect ts::graphics::Texture::sub_texture_rect(int id) const
{
    auto max_texture_size = sf::Texture::getMaximumSize();
    auto x_count = (total_size_.x - 1) / max_texture_size + 1;

    Int_rect result;
    result.left = (id % x_count) * max_texture_size;
    result.width = std::min(max_texture_size, total_size_.x - result.left);

    result.top = (id / x_count) * max_texture_size;
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

void ts::graphics::Texture::load_from_image(const sf::Image& image, Rect<int> rect)
{
    auto image_size = image.getSize();
    if (rect.width == 0 || rect.height == 0) {
        rect = Rect<int>(0, 0, image_size.x, image_size.y);
    }

    auto max_texture_size = sf::Texture::getMaximumSize();

    // Get the number of sub-textures
    auto x_count = rect.width ? (rect.width - 1) / max_texture_size + 1 : 0;
    auto y_count = rect.height ? (rect.height - 1) / max_texture_size + 1 : 0;
    auto sub_texture_count = x_count * y_count;

    sub_textures_.resize(sub_texture_count);
    total_size_ = image_size;

    for (unsigned int id = 0; id < sub_texture_count; ++id) {
        auto& texture = sub_textures_[id];
        auto sub_rect = intersection(sub_texture_rect(id), rect);

        sf::IntRect sf_rect(sub_rect.left, sub_rect.top, sub_rect.width, sub_rect.height);
        if (!texture.loadFromImage(image, sf_rect)) {
            throw std::runtime_error("could not create texture");
        }
    }
}