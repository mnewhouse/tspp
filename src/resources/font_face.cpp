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
#include "font_face.hpp"

#include <SFML/Graphics.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace ts
{
    namespace resources
    {        
        struct Page_row
        {
            std::int32_t y_offset;
            std::int32_t width;
            std::int32_t height;
        };

        struct Page
        {
            Page();

            Int_rect allocate_glyph_space(Vector2u size);
            std::uint32_t row_start;

            sf::Texture texture;

            std::vector<Page_row> rows;
            std::unordered_map<utf8::uint32_t, Glyph> glyphs;       
        };
    }
}

struct ts::resources::impl::Font_face
{
    Font_face(const void* data, std::size_t data_size);
    ~Font_face();

    const Glyph& get_glyph(utf8::uint32_t code_point, std::uint32_t character_size);
    const Glyph& load_glyph(utf8::uint32_t code_point, std::uint32_t character_size);

    const sf::Texture* texture(std::uint32_t character_size) const;

    void set_character_size(std::uint32_t character_size) const;
    Vector2i kerning(utf8::uint32_t first, utf8::uint32_t second, std::uint32_t character_size);

    std::uint32_t default_line_height(std::uint32_t character_size) const;

private:
    FT_Library library;
    FT_Face face;

    std::unordered_map<std::uint32_t, Page> page_map;
    std::vector<std::uint8_t> pixel_buffer;
};


ts::resources::Font_face::Font_face(const void* data, std::size_t data_size)
: impl_(std::make_shared<impl::Font_face>(data, data_size))
{
}

ts::resources::Font_face::operator bool() const
{
    return impl_ != nullptr;
}

ts::Vector2i ts::resources::Font_face::kerning(utf8::uint32_t first, utf8::uint32_t second, std::uint32_t character_size) const
{
    return impl_->kerning(first, second, character_size);
}

const sf::Texture* ts::resources::Font_face::texture(std::uint32_t character_size) const
{
    return impl_->texture(character_size);
}

const ts::resources::Glyph& ts::resources::Font_face::glyph(utf8::uint32_t code_point, std::uint32_t character_size) const
{
    return impl_->get_glyph(code_point, character_size);
}

std::uint32_t ts::resources::Font_face::default_line_height(std::uint32_t character_size) const
{
    return impl_->default_line_height(character_size);
}

ts::Vector2i ts::resources::impl::Font_face::kerning(utf8::uint32_t first, utf8::uint32_t second, std::uint32_t character_size)
{
    if (first && second && FT_HAS_KERNING(face))
    {
        set_character_size(character_size);

        auto first_index = FT_Get_Char_Index(face, first);
        auto second_index = FT_Get_Char_Index(face, second);

        FT_Vector kerning;
        FT_Get_Kerning(face, first_index, second_index, FT_KERNING_DEFAULT, &kerning);
        
        return Vector2i(kerning.x >> 6, kerning.y >> 6);            
    }

    return Vector2i();
}


// Implementation
ts::resources::impl::Font_face::Font_face(const void* data, std::size_t data_size)
{
    FT_Init_FreeType(&library);

    if (FT_New_Memory_Face(library, static_cast<const FT_Byte*>(data), static_cast<FT_Long>(data_size), 0, &face) != 0)
    {
        throw std::runtime_error("failed to load font from memory (could not create font face)");
    }

    // Select the unicode character map
    if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0)
    {
        throw std::runtime_error("failed to load font from memory (could not set the Unicode character set)");
    }
}

ts::resources::impl::Font_face::~Font_face()
{
    if (face)
    {
        FT_Done_Face(face);
    }
    
    if (library)
    {
        FT_Done_FreeType(library);
    }    
}

void ts::resources::impl::Font_face::set_character_size(std::uint32_t character_size) const
{
    auto current_size = face->size->metrics.x_ppem;
    if (current_size != character_size)
    {
        FT_Set_Pixel_Sizes(face, 0, character_size);
    }
}

std::uint32_t ts::resources::impl::Font_face::default_line_height(std::uint32_t character_size) const
{
    set_character_size(character_size);

    return face->size->metrics.height >> 6;
}


const ts::resources::Glyph& ts::resources::impl::Font_face::get_glyph(utf8::uint32_t code_point, std::uint32_t character_size)
{
    auto& page = page_map[character_size];

    auto glyph_it = page.glyphs.find(code_point);
    if (glyph_it != page.glyphs.end())
    {
        return glyph_it->second;
    }

    else
    {
        return load_glyph(code_point, character_size);
    }
}

const ts::resources::Glyph& ts::resources::impl::Font_face::load_glyph(utf8::uint32_t code_point, std::uint32_t character_size)
{
    auto glyph_slot = face->glyph;
    auto& bitmap = glyph_slot->bitmap;

    set_character_size(character_size);

    if (FT_Load_Char(face, code_point, FT_LOAD_RENDER) != 0)
    {
        throw std::runtime_error("failed to load char");
    }

    auto& page = page_map[character_size];
    auto& glyph = page.glyphs[code_point];
   
    glyph.advance_offset = { glyph_slot->advance.x >> 6, glyph_slot->advance.y >> 6 };

    Vector2u dimensions(bitmap.width, bitmap.rows);   
    if (dimensions.x != 0 && dimensions.y != 0)
    {
        const int padding = 1;
        dimensions += padding * 2;

        glyph.bounds.left = glyph_slot->bitmap_left - padding;
        glyph.bounds.top = -glyph_slot->bitmap_top - padding;
        glyph.bounds.width = dimensions.x;
        glyph.bounds.height = dimensions.y;

        glyph.texture_rect = page.allocate_glyph_space(dimensions);

        pixel_buffer.resize(dimensions.x * dimensions.y * 4, 255);
        const auto* pixels = bitmap.buffer;
        auto* dest = &pixel_buffer[0];

        for (std::int32_t y = 0; y != bitmap.rows; ++y)
        {            
            auto* src = pixels;
            for (std::int32_t x = 0; x != bitmap.width; ++x)
            {
                dest += 3;
                *dest++ = *src++;
            }

            pixels += bitmap.pitch;
        }

        auto x = glyph.texture_rect.left + padding;
        auto y = glyph.texture_rect.top + padding;
        auto w = glyph.texture_rect.width - 2 * padding;
        auto h = glyph.texture_rect.height - 2 * padding;
        page.texture.update(pixel_buffer.data(), w, h, x, y);
    }
    
    return glyph;
}

const sf::Texture* ts::resources::impl::Font_face::texture(std::uint32_t character_size) const
{
    auto it = page_map.find(character_size);
    if (it == page_map.end())
    {
        return nullptr;
    }

    return &it->second.texture;
}

ts::resources::Page::Page()
: row_start(0)
{
    sf::Image image;
    image.create(256, 256, sf::Color(255, 255, 255, 0));
    texture.loadFromImage(image);
}

ts::Int_rect ts::resources::Page::allocate_glyph_space(Vector2u dimensions)
{
    Page_row* row = nullptr;
    double best_ratio = 0.0;

    auto texture_size = texture.getSize();
    
    for (auto& current_row : rows)
    {
        auto ratio = static_cast<double>(dimensions.y) / current_row.height;

        // Check for the best ratio that is not higher than 1
        if (ratio >= 0.7 && ratio <= 1.0 && ratio > best_ratio)
        {
            // Is there enough horizontal space on the row?
            if (texture_size.x - current_row.width >= dimensions.x)
            {
                row = &current_row;
                best_ratio = ratio;
            }
        }
    }

    if (!row)
    {
        // Insert a new row. Test for vertical space
        std::uint32_t row_height = dimensions.y + (dimensions.y / 10) + 1;
        auto texture_size = texture.getSize();
        auto max_texture_size = texture.getMaximumSize();

        while (row_start + row_height >= texture_size.y)
        {
            // Double the size
            texture_size *= 2U;
            if (texture_size.x <= max_texture_size && texture_size.y <= max_texture_size)
            {
                sf::Image image;
                image.create(texture_size.x, texture_size.y, sf::Color(255, 255, 255, 0));
                image.copy(texture.copyToImage(), 0, 0);
                texture.loadFromImage(image);
            }

            else
            {
                return Int_rect();
            }
        }

        rows.emplace_back();
        auto& inserted_row = rows.back();
        inserted_row.y_offset = row_start;
        inserted_row.width = 0;
        inserted_row.height = row_height;

        row_start += row_height;
        row = &inserted_row;
    }

    Int_rect result(row->width, row->y_offset, dimensions.x, dimensions.y);

    row->width += dimensions.x;

    return result;
}

