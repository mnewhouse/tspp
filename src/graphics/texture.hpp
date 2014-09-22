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

#pragma once

#ifndef GRAPHICS_TEXTURE_HPP
#define GRAPHICS_TEXTURE_HPP

#include <SFML/Graphics.hpp>

namespace ts
{

    namespace graphics
    {
        struct Sub_texture
        {
            sf::Texture texture;
            Int_rect rect;
        };

        class Texture
        {
        public:
            explicit Texture(const sf::Image& image, Int_rect area = Int_rect{});
            explicit Texture(const std::string& file_name, Int_rect area = Int_rect{});

            Texture(Texture&& texture);
            Texture& operator=(Texture&& texture);

            void load_from_file(const std::string& file_name, Int_rect area = Int_rect{});
            void load_from_image(const sf::Image& image, Int_rect area = Int_rect{});

            Vector2u size() const;

            const std::vector<Sub_texture>& sub_textures() const;


        private:
            Int_rect sub_texture_rect(std::size_t) const;

            std::vector<Sub_texture> sub_textures_;
            sf::Vector2u total_size_;
        };

        template <typename Operation>
        void for_each_sub_texture(const Texture& texture, Operation op, Int_rect area = Int_rect{})
        {
            if (area.width == 0 || area.height == 0) 
            {
                auto texture_size = texture.size();
                area.width = texture_size.x;
                area.height = texture_size.y;
                area.left = 0;
                area.top = 0;
            }

            const auto& sub_textures = texture.sub_textures();
            for (auto& sub_texture : sub_textures) 
            {
                auto rect = intersection(sub_texture.rect, area);

                if (rect.width && rect.height) 
                {
                    rect.left -= sub_texture.rect.left;
                    rect.top -= sub_texture.rect.top;

                    op(sub_texture, rect);
                }
            }
        }
    }
}

#endif