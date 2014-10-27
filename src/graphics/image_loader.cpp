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
#include "image_loader.hpp"

ts::graphics::Image_load_error::Image_load_error(utf8_string file_path)
: std::logic_error("failed to open image " + file_path.string()),
  file_path_(std::move(file_path))
{
}

const ts::utf8_string& ts::graphics::Image_load_error::file_path() const
{
    return file_path_;
}

const sf::Image* ts::graphics::Image_loader::load_from_file(const utf8_string& file_name, std::nothrow_t)
{
    auto it = image_map_.find(file_name);
    if (it != image_map_.end())
    {
        return &it->second;
    }

    return load_from_file_impl(file_name);
}

const sf::Image& ts::graphics::Image_loader::load_from_file(const utf8_string& file_name)
{
    auto* image = load_from_file(file_name, std::nothrow);
    if (!image)
    {
        throw Image_load_error(file_name);
    }

    return *image;
}

const sf::Image* ts::graphics::Image_loader::load_from_file_impl(const utf8_string& file_name)
{
    boost::filesystem::ifstream stream(file_name.string(), std::ios::binary | std::ios::in);
    if (stream)
    {
        stream.seekg(0, std::ios::end);
        std::size_t size = static_cast<std::size_t>(stream.tellg());
        file_buffer_.resize(size);

        stream.seekg(0);
        stream.read(file_buffer_.data(), file_buffer_.size());

        auto& image = image_map_[file_name];
        if (image.loadFromMemory(file_buffer_.data(), file_buffer_.size()))
        {
            return &image;
        }

        image_map_.erase(file_name);
    }

    return nullptr;
}