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
#include "text.hpp"  

ts::graphics::Text::Text(utf8_string text, Font_face font, std::uint32_t character_size)
: text_(std::move(text)), font_(font), character_size_(character_size),
  color_(sf::Color::White),
  line_height_(font.default_line_height(character_size)),
  word_wrap_width_(0)
{
    geometry_.dirty = true;
}

void ts::graphics::Text::set_position(Vector2<double> position)
{
    geometry_.geometry.position = position;
}

ts::Vector2<double> ts::graphics::Text::position() const
{
    return geometry_.geometry.position;
}

void ts::graphics::Text::set_color(sf::Color color)
{
    color_ = color;

    for (auto& vertex : geometry_.geometry.vertices)
    {
        vertex.color = color;
    }
}

void ts::graphics::Text::set_font(Font_face font)
{
    font_ = font;
    geometry_.dirty = true;
}

void ts::graphics::Text::set_line_height(std::uint32_t line_height)
{
    line_height_ = line_height;
    geometry_.dirty = true;
}

void ts::graphics::Text::set_text(utf8_string text)
{
    text_ = std::move(text);
    geometry_.dirty = true;
}

void ts::graphics::Text::set_character_size(std::uint32_t character_size)
{
    character_size_ = character_size;
    geometry_.dirty = true;
}

const ts::graphics::Geometry& ts::graphics::Text::geometry() const
{
    if (geometry_.dirty)
    {
        update_geometry();
    }

    return geometry_.geometry;
}

const ts::Double_rect& ts::graphics::Text::bounds() const
{
    if (geometry_.dirty)
    {
        update_geometry();
    }

    return geometry_.bounds;
}

double ts::graphics::Text::base_line(std::size_t character_index) const
{
    double base = line_height_;

    if (geometry_.dirty)
    {
        update_geometry();
    }
    
    auto it = std::lower_bound(geometry_.line_breaks.begin(), geometry_.line_breaks.end(), character_index);
    auto line_number = it - geometry_.line_breaks.begin();

    return base + (base * line_number);
}

ts::Vector2<double> ts::graphics::Text::character_position(std::size_t index) const
{
    if (geometry_.dirty)
    {
        update_geometry();
    }

    if (index >= geometry_.character_offsets.size())
    {
        return Vector2<double>();
    }

    Vector2<double> result;
    result.x = geometry_.character_offsets[index];
    result.y = base_line(index);

    return result;
}

void ts::graphics::Text::update_geometry() const
{
    geometry_.dirty = false;

    auto& vertices = geometry_.geometry.vertices;
    vertices.clear();

    if (!font_) return;

    Vector2f position{ 0.0f, std::floor((line_height_ + character_size_) * 0.5f) };

    auto append_vertices = [](std::vector<sf::Vertex>& vertices, Vector2f position, sf::Color color, 
                              const Int_rect& bounds, const Int_rect& texture_rect)
    {        
        auto left = static_cast<float>(bounds.left);
        auto top = static_cast<float>(bounds.top);
        auto right = static_cast<float>(bounds.right());
        auto bottom = static_cast<float>(bounds.bottom());

        auto tex_left = static_cast<float>(texture_rect.left);
        auto tex_top = static_cast<float>(texture_rect.top);
        auto tex_right = static_cast<float>(texture_rect.right());
        auto tex_bottom = static_cast<float>(texture_rect.bottom());

        sf::Vertex vertex;
        vertex.color = color;

        vertex.position = { position.x + left, position.y + top };
        vertex.texCoords = { tex_left, tex_top };
        vertices.push_back(vertex);

        vertex.position.x = position.x + right;
        vertex.texCoords.x = tex_right;
        vertices.push_back(vertex);

        vertex.position.y = position.y + bottom;
        vertex.texCoords.y = tex_bottom;
        vertices.push_back(vertex);

        vertex.position.x = position.x + left;
        vertex.texCoords.x = tex_left;
        vertices.push_back(vertex);
    };

    geometry_.line_breaks.clear();
    geometry_.character_offsets.clear();

    utf8::uint32_t previous_char = 0;
   
    auto word_boundary = text_.begin(), line_boundary = word_boundary;
    std::size_t word_boundary_index = 0;
    
    // Loop through all code points
    for (auto it = text_.begin(), end = text_.end(); it != end; )
    {
        std::uint32_t code_point = *it++;
        position += font_.kerning(previous_char, code_point, character_size_);

        auto& glyph = font_.glyph(code_point, character_size_);

        if (position.x == 0.0 && glyph.bounds.left < 0)
        {
            position.x = static_cast<float>(-glyph.bounds.left);
        }
        
        geometry_.character_offsets.push_back(position.x);
        append_vertices(vertices, position, color_, glyph.bounds, glyph.texture_rect);

        // Test for combining marks
        for (std::int32_t ccc; it != end && (ccc = unicode::combining_class(*it)) != 0; ++it)
        {
            auto& combining_mark = font_.glyph(*it, character_size_);

            // Center the combining mark
            auto centered_position = position;
            centered_position.x += (glyph.bounds.left - combining_mark.bounds.left) + ((glyph.bounds.width - combining_mark.bounds.width) / 2);

            geometry_.character_offsets.push_back(position.x);
            append_vertices(vertices, centered_position, color_, combining_mark.bounds, combining_mark.texture_rect);
        }

        position += glyph.advance_offset;

        if (word_wrap_width_ != 0)
        {           
            if (unicode::is_space(code_point))
            {
                word_boundary = it;
                word_boundary_index = vertices.size();
            }

            else if (position.x >= word_wrap_width_ && word_boundary != line_boundary)
            {
                // There's been an overflow, do some word-wrapping
                // This is done by erasing the vertices that were inserted
                // after the word boundary, and setting the cursor to the start of
                // the new line.

                it = word_boundary;

                line_boundary = word_boundary;
                vertices.resize(word_boundary_index);

                geometry_.character_offsets.resize(word_boundary_index);
                geometry_.line_breaks.push_back(word_boundary_index);

                position.x = 0.0f;
                position.y += line_height_;
            }
        }

        previous_char = code_point;
    }

    geometry_.character_offsets.push_back(position.x);

    geometry_.geometry.texture = font_.texture(character_size_);
    geometry_.geometry.primitive_type = sf::Quads;

    auto& bounds = geometry_.bounds;
    bounds.width = 0.0;
    bounds.height = 0.0;

    if (!vertices.empty())
    {
        auto& vertex = vertices.front();
        bounds.left = vertex.position.x;
        bounds.top = vertex.position.y;
    }

    for (auto& vertex : vertices)
    {
        auto position = vertex.position;
        if (position.x < bounds.left) bounds.left = position.x;
        if (position.y < bounds.top) bounds.top = position.y;

        if (position.x > bounds.right()) bounds.width = position.x - bounds.left;
        if (position.y > bounds.bottom()) bounds.height = position.y - bounds.top;
    }
}

const ts::utf8_string& ts::graphics::Text::text() const
{
    return text_;
}

double ts::graphics::Text::line_height() const
{
    return line_height_;
}

std::uint32_t ts::graphics::Text::character_size() const
{
    return character_size_;
}

sf::Color ts::graphics::Text::color() const
{
    return color_;
}