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

#include "pattern.hpp"

extern "C" {
#include <png.h>
}

#include <cstdio>
#include <algorithm>
#include <iostream>

#include "core/utility.hpp"

#include "collision_mask.hpp"

ts::resources::Pattern::Pattern(const std::string& file_name, Int_rect rect)
{
    load_from_file(file_name, rect);
}

ts::resources::Pattern::Pattern(Vector2u size)
: bytes_(size.x * size.y, 0), size_(size)
{
}

void ts::resources::Pattern::load_from_file(const std::string& file_name, Int_rect rect)
{
    auto file_handle = std::fopen(file_name.c_str(), "rb");
    if (!file_handle) {
        throw std::runtime_error("could not open pattern file " + file_name + ".");
    }

    auto destroy_file_handle = scope_exit([file_handle]() { std::fclose(file_handle); });    

    auto read_png_file = [this, &rect](FILE* file)
    {
        unsigned char sig[8] = {};

        std::fread(sig, 1, 8, file);
        if (!png_check_sig(sig, 8)) return false;

        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png_ptr) return false;

        auto destroy_read_struct = scope_exit([&]() { png_destroy_read_struct(&png_ptr, nullptr, nullptr); });

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) return false;

        if (setjmp(png_jmpbuf(png_ptr))) return false;

        auto destroy_info_struct = scope_exit([&]() { png_destroy_info_struct(png_ptr, &info_ptr); });

        png_init_io(png_ptr, file);
        png_set_sig_bytes(png_ptr, 8);
        png_read_info(png_ptr, info_ptr);

        if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_PALETTE)
            return false;

        auto image_width = png_get_image_width(png_ptr, info_ptr);
        auto image_height = png_get_image_height(png_ptr, info_ptr);

        std::vector<Terrain_id> byte_array(image_width * image_height);
        std::vector<png_bytep> row_pointers(image_height);


        {
            std::size_t row = 0;
            std::generate(row_pointers.begin(), row_pointers.end(), 
                [&]() { return &byte_array[row++ * image_width]; }
            );
        }

        png_read_image(png_ptr, &row_pointers[0]);
        png_read_end(png_ptr, nullptr);

        if (rect.width == 0) {
            rect.width = image_width;
            rect.left = 0;
        }

        if (rect.height == 0) {
            rect.height = image_height;
            rect.top = 0;
        }

        if (image_width == rect.width && rect.left == 0 &&
            image_height == rect.height && rect.top == 0) 
        {
            bytes_ = std::move(byte_array);
        }

        else {
            for (auto y = rect.top; y != rect.bottom(); ++y) {
                for (auto x = rect.left; x != rect.right(); ++x) {
                    auto idx = x + y * image_width;
                    (*this)(x, y) = byte_array[idx];
                }
            }            
        }

        size_.x = rect.width;
        size_.y = rect.height;
        
        return true;
    };

    if (!read_png_file(file_handle)) {
        throw std::runtime_error("invalid pattern file; must be an indexed PNG image");
    }
}


const ts::resources::Terrain_id& ts::resources::Pattern::operator()(std::size_t x, std::size_t y) const
{
    return bytes_[x + y * size().x];
}

ts::resources::Terrain_id& ts::resources::Pattern::operator()(std::size_t x, std::size_t y)
{
    return bytes_[x + y * size().x];
}

ts::Vector2u ts::resources::Pattern::size() const
{
    return size_;
}