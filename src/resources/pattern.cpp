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
#include "pattern.hpp"

extern "C"
{
#include <png.h>
}

namespace ts
{
    namespace png
    {
        struct Info_struct
        {
            explicit Info_struct(png_structp read_ptr, png_infop info_ptr)
                : read_ptr_(read_ptr),
                  info_ptr_(info_ptr)
            {
            }

            Info_struct()
                : read_ptr_(png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr)),
                  info_ptr_(png_create_info_struct(read_ptr_))
            {

            }

            ~Info_struct()
            {
                if (info_ptr_)
                {
                    png_destroy_info_struct(read_ptr_, &info_ptr_);
                }
                
                if (read_ptr_)
                {
                    png_destroy_read_struct(&read_ptr_, nullptr, nullptr);
                }
                    
            }

            Info_struct(const Info_struct&) = delete;
            Info_struct& operator=(const Info_struct&) = delete;

            png_structp& read_ptr()
            {
                return read_ptr_;
            }
            
            png_infop& info_ptr()
            {
                return info_ptr_;
            }
            
            explicit operator bool() const
            {
                return read_ptr_ && info_ptr_;
            }


        private:
            png_structp read_ptr_;
            png_infop info_ptr_;
        };

        struct Reader_struct
        {
            const unsigned char* data_;
            const unsigned char* end_;            
        };

        void read_using_reader_struct(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead);
    }
}

ts::resources::Pattern_load_error::Pattern_load_error(utf8_string file_path)
: std::logic_error("could not load pattern file " + file_path.string() + " (must be paletted PNG image)"),
  file_path_(std::move(file_path))
{
}

ts::resources::Pattern::Pattern(const utf8_string& file_name, Int_rect rect)
{
    load_from_file(file_name, rect);
}

ts::resources::Pattern::Pattern(Vector2u size)
: size_(size), bytes_(size.x * size.y, 0)
{
}

ts::resources::Pattern::Pattern(Pattern&& other)
: size_(other.size_),
  bytes_(std::move(other.bytes_))
{
    other.size_ = {};
}

ts::resources::Pattern& ts::resources::Pattern::operator=(Pattern&& other)
{
    size_ = other.size_;
    bytes_ = std::move(other.bytes_);

    other.size_ = {};
    return *this;
}

void ts::png::read_using_reader_struct(png_structp png_ptr, png_bytep out_bytes, png_size_t byte_count)
{
    auto* reader = static_cast<Reader_struct*>(png_ptr->io_ptr);
    if (reader)
    {
        png_size_t bytes_left = reader->end_ - reader->data_;
        if (bytes_left >= byte_count)
        {
            std::copy(reader->data_, reader->data_ + byte_count, out_bytes);
            reader->data_ += byte_count;
        }
    }
}

void ts::resources::Pattern::load_from_file(const utf8_string& file_name, Int_rect rect)
{
    boost::filesystem::basic_ifstream<unsigned char> stream(file_name.string(), std::ifstream::in | std::ifstream::binary);
    if (stream)
    {
        auto file_contents = core::read_stream_contents(stream);
        if (file_contents.size() && png_check_sig(file_contents.data(), 8))
        {
            png::Info_struct png_info;
            auto& read_ptr = png_info.read_ptr();
            auto& info_ptr = png_info.info_ptr();

            if (png_info && setjmp(png_jmpbuf(read_ptr)) == 0)
            {
                png::Reader_struct reader;
                reader.data_ = file_contents.data() + 8;
                reader.end_ = file_contents.data() + file_contents.size();

                png_set_read_fn(read_ptr, static_cast<void*>(&reader), png::read_using_reader_struct);
                png_set_sig_bytes(read_ptr, 8);
                png_read_info(read_ptr, info_ptr);

                // Must be paletted image
                if (png_get_color_type(read_ptr, info_ptr) == PNG_COLOR_TYPE_PALETTE)
                {
                    std::size_t image_width = png_get_image_width(read_ptr, info_ptr);
                    std::size_t image_height = png_get_image_height(read_ptr, info_ptr);

                    std::vector<png_byte> byte_array(image_width * image_height);
                    std::vector<png_bytep> row_pointers(image_height);

                    for (std::size_t row = 0; row != image_height; ++row)
                    {
                        row_pointers[row] = &byte_array[row * image_width];
                    }

                    if (rect.width == 0)
                    {
                        rect.left = 0;
                        rect.width = image_width;
                    }

                    if (rect.height == 0)
                    {
                        rect.top = 0;
                        rect.height = image_height;
                    }

                    png_read_image(read_ptr, row_pointers.data());
                    png_read_end(read_ptr, info_ptr);

                    if (image_width >= static_cast<std::size_t>(rect.right()) && image_height >= static_cast<std::size_t>(rect.bottom()))
                    {
                        // Resize
                        resize(rect.width, rect.height);

                        // And copy the bytes over
                        auto* data_ptr = &bytes_[0];
                        for (std::uint32_t y = rect.top, bottom = rect.bottom(); y != bottom; ++y)
                        {
                            auto source_ptr = &byte_array[y * image_width + rect.left];

                            for (auto row_end = data_ptr + rect.width; data_ptr != row_end; ++data_ptr, ++source_ptr)
                            {
                                *data_ptr = *source_ptr;
                            }
                        }

                        // Only if we reach this point, we can avoid throwing an exception.
                        // Pretty god-awful, so many levels of indentation.
                        return;
                    }
                }
            }
        }        
    }

    throw Pattern_load_error(file_name);
}

ts::resources::Pattern::const_iterator ts::resources::Pattern::begin() const
{
    return bytes_.data();
}

ts::resources::Pattern::const_iterator ts::resources::Pattern::end() const
{
    return bytes_.data();
}

ts::resources::Pattern::const_iterator ts::resources::Pattern::row_begin(std::uint32_t y) const
{
    return bytes_.data() + y * size_.x;
}

ts::resources::Pattern::const_iterator ts::resources::Pattern::row_end(std::uint32_t y) const
{
    return bytes_.data() + y * size_.x + size_.x;
}


ts::resources::Pattern::iterator ts::resources::Pattern::begin()
{
    return bytes_.data();
}

ts::resources::Pattern::iterator ts::resources::Pattern::end()
{
    return bytes_.data();
}

ts::resources::Pattern::iterator ts::resources::Pattern::row_begin(std::uint32_t y)
{
    return bytes_.data() + y * size_.x;
}

ts::resources::Pattern::iterator ts::resources::Pattern::row_end(std::uint32_t y)
{
    return bytes_.data() + y * size_.x + size_.x;
}

const ts::resources::Terrain_id& ts::resources::Pattern::operator()(std::uint32_t x, std::uint32_t y) const
{
    return bytes_[x + y * size_.x];
}

ts::resources::Terrain_id& ts::resources::Pattern::operator()(std::uint32_t x, std::uint32_t y)
{
    return bytes_[x + y * size_.x];
}

ts::Vector2u ts::resources::Pattern::size() const
{
    return size_;
}

void ts::resources::Pattern::resize(Vector2u new_size)
{
    bytes_.resize(new_size.x * new_size.y);

    size_.x = new_size.x;
    size_.y = new_size.y;
}

void ts::resources::Pattern::resize(std::uint32_t width, std::uint32_t height)
{
    resize(Vector2u(width, height));
}