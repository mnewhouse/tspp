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
#include "track_builder.hpp"

#include "graphics/texture_loader.hpp"
#include "graphics/texture.hpp"

#include "resources/track.hpp"
#include "resources/include_path.hpp"


ts::game::Track_component::Track_component(std::vector<sf::Vertex> vertices, const sf::Texture* texture, std::uint32_t level)
: vertices(std::move(vertices)), texture(texture), level(level)
{
}

ts::Int_rect ts::game::impl::Target_texture::allocate_tile_space(Vector2i size)
{
    if (size.x > texture_size.x || row_start + size.y > texture_size.y)
    {
        return Int_rect();
    }

    // Find a suitable row
    auto found_row = std::find_if(texture_rows.begin(), texture_rows.end(), 
                                  [size](const Texture_row& row)
    {
        return row.free_space > size.x && size.y < row.area.height - 1 &&
            size.y * 10 >= row.area.height * 7;
    });

    if (found_row == texture_rows.end())
    {
        texture_rows.emplace_back();

        found_row = std::prev(texture_rows.end());
        found_row->area.top = row_start;
        found_row->area.height = size.y + 2;
        found_row->area.left = 0;
        found_row->area.width = texture_size.x;
        found_row->free_space = found_row->area.width;

        row_start += found_row->area.height;
    }

    Vector2i tile_position(found_row->area.right() - found_row->free_space, found_row->area.top);

    found_row->free_space -= size.x + 2;

    return Int_rect(tile_position.x, tile_position.y, size.x, size.y);
}

void ts::game::impl::Target_texture::deallocate_tile_space(Int_rect area)
{
    auto row_it = std::find_if(texture_rows.begin(), texture_rows.end(), 
                            [area](const Texture_row& row)
    {
        return row.area.top == area.top && area.left >= row.area.left && area.right() <= row.area.right();
    });

    if (row_it != texture_rows.end())
    {
        Texture_row remainder;
        remainder.area.left = area.right();
        remainder.area.width = row_it->area.right() - remainder.area.left;
        remainder.area.top = row_it->area.top;
        remainder.area.height = row_it->area.height;
        remainder.free_space = row_it->free_space;
       

        row_it->free_space = area.width;
        row_it->area.width -= remainder.area.width;

        texture_rows.push_back(remainder);      
    }
}

ts::game::Track_scene::Track_scene(Vector2u track_size)
: track_size(track_size)
{
}

ts::game::Track_scene::Track_scene(Track_scene&& other)
: components(std::move(other.components)),
  textures(std::move(other.textures)),
  track_size(other.track_size)  
{
    other.track_size = {};
}

ts::game::Track_scene& ts::game::Track_scene::operator=(Track_scene&& other)
{
    components = std::move(other.components);
    textures = std::move(other.textures);
    track_size = other.track_size;
    other.track_size = {};
    return *this;
}


ts::game::Track_builder::Track_builder(const resources::Track& track)
: track_(track), result_(track.size())
{
    create_new_target_texture();
}

void ts::game::Track_builder::preload_image(const utf8_string& path)
{
    //const auto& resolved_path = resolve_path(path);
    image_loader_.load_from_file(path);
}

/*
const ts::utf8_string& ts::game::Track_builder::resolve_path(const utf8_string& path)
{
    auto it = precomputed_paths_.find(path);
    if (it == precomputed_paths_.end())
    {
        auto resolved_path = track_.find_include_path(path);
        it = precomputed_paths_.emplace(path, std::move(resolved_path)).first;
    }

    return it->second;
}
*/

void ts::game::Track_builder::add_large_tile(const resources::Placed_tile& placed_tile, Int_rect image_rect)
{
    auto texture_size = current_target_texture_->texture_size;

    const auto* tile_def = placed_tile.tile_def;
    const auto& image_path = tile_def->image_file();
    auto mapping_it = large_tile_mapping_.find(tile_def);
    if (mapping_it != large_tile_mapping_.end())
    {
        for (auto target_texture : mapping_it->second)
        {
            add_tile_entry(placed_tile, target_texture);
        }
    }

    else
    {
        auto& large_tile_mapping = large_tile_mapping_[tile_def];

        for (std::int32_t y = image_rect.top, bottom = image_rect.bottom(); y < bottom; y += texture_size.y)
        {
            for (std::int32_t x = image_rect.left, right = image_rect.right(); x < right; x += texture_size.x)
            {
                auto target_texture = create_new_target_texture();

                Vector2i size(std::min(right - x, texture_size.x), std::min(bottom - y, texture_size.y));

                auto allocated_space = target_texture->allocate_tile_space(size);
                auto& tile_list = target_texture->tile_placement[image_path];
                auto& tile_mapping = target_texture->tile_mapping;

                impl::Tile_placement tile_placement;
                tile_placement.target_rect = allocated_space;
                tile_placement.source_rect = Int_rect(x, y, size.x, size.y);
                tile_list.push_back(tile_placement);

                // Default construct tile mapping element
                tile_mapping[tile_def];
                large_tile_mapping.push_back(target_texture);
                
                add_tile_entry(placed_tile, target_texture);
            }
        }
    }
}

std::pair<ts::game::impl::Target_texture*, ts::Int_rect> ts::game::Track_builder::allocate_tile_space(Int_rect image_rect)
{
    Vector2i area_size(image_rect.width, image_rect.height);

    auto target_texture = current_target_texture_;    
    auto allocated_space = target_texture->allocate_tile_space(area_size);

    if (allocated_space.width == 0 || allocated_space.height == 0)
    {
        target_texture = create_new_target_texture();
        allocated_space = target_texture->allocate_tile_space(area_size);

        // We already asserted that texture_size >= area_size,
        // so allocating space on the newly created texture should never fail.

        assert(allocated_space.width != 0 && allocated_space.height != 0);
    }

    return std::make_pair(target_texture, allocated_space);
}

std::size_t ts::game::Track_builder::step_count() const
{
    return track_.placed_tiles().size() * 2;
}

void ts::game::Track_builder::generate_texture(impl::Target_texture& target_texture)
{
    Vector2i texture_size(0, std::min(target_texture.row_start, target_texture.texture_size.y));
    for (const auto& row : target_texture.texture_rows)
    {
        auto right = row.area.right();
        if (right > texture_size.x) texture_size.x = right;
    }

    image_buffer_.create(texture_size.x, texture_size.y, sf::Color::Transparent);

    for (const auto& file_info : target_texture.tile_placement)
    {
        const auto& resolved_path = file_info.first;
        const auto& tile_list = file_info.second;

        const sf::Image& source = image_loader_.load_from_file(resolved_path);
        for (auto& tile_placement : tile_list)
        {
            const auto& source_rect = tile_placement.source_rect;
            const auto& target_rect = tile_placement.target_rect;

            image_buffer_.copy(source, target_rect.left, target_rect.top,
                               sf::IntRect(source_rect.left, source_rect.top, source_rect.width, source_rect.height));
        }
    }

    auto texture_ptr = std::make_unique<sf::Texture>();
    if (texture_ptr->loadFromImage(image_buffer_))
    {
        target_texture.texture = std::move(texture_ptr);
    }
}

void ts::game::Track_builder::build_tile_mapping(impl::Target_texture& target_texture)
{
    for (auto& tile_mapping : target_texture.tile_mapping)
    {
        const auto* tile_def = tile_mapping.first;
        const auto& image_path = tile_def->image_file();

        auto map_it = target_texture.tile_placement.find(image_path);
        const sf::Image& image = image_loader_.load_from_file(image_path);

        if (map_it != target_texture.tile_placement.end())
        {
            const auto& source_rect = tile_def->image_rect;
            const auto& area_list = map_it->second;

            auto search_result = std::find_if(area_list.begin(), area_list.end(), 
                                              [source_rect](const impl::Tile_placement& tile_placement)
            {
                return intersection(tile_placement.source_rect, source_rect) == source_rect;
            });

            if (search_result == area_list.end())
            {
                search_result = std::find_if(area_list.begin(), area_list.end(),
                                            [source_rect](const impl::Tile_placement& tile_placement)
                {
                    return intersects(tile_placement.source_rect, source_rect);
                });
            }

            if (search_result != area_list.end())
            {
                auto intersect_area = intersection(search_result->source_rect, source_rect);

                auto x_offset = intersect_area.left - search_result->source_rect.left;
                auto y_offset = intersect_area.top - search_result->source_rect.top;

                auto& mapping = tile_mapping.second;
                mapping.image = &image;
                mapping.source_rect = intersect_area;
                mapping.target_rect.left = search_result->target_rect.left + x_offset;
                mapping.target_rect.top = search_result->target_rect.top + y_offset;
                mapping.target_rect.width = intersect_area.width;
                mapping.target_rect.height = intersect_area.height;
            }
        }
    }
}

void ts::game::Track_builder::build_scene(std::function<void()> step_operation)
{
    for (const auto& tile : track_.placed_tiles())
    {
        add_tile(tile);

        if (step_operation) step_operation();
    }

    // Need to map tile id to target texture area

    target_texture_list_.remove_if([](const impl::Target_texture& target_texture)
    {
        return target_texture.texture_rows.size() == 0;
    });

    for (auto& target_texture : target_texture_list_)
    {
        generate_texture(target_texture);
        build_tile_mapping(target_texture);
    }

    for (const auto& tile_entry : tile_entries_)
    {
        add_tile_to_scene(tile_entry);

        if (step_operation) step_operation();
    }

    for (auto& target_texture : target_texture_list_)
    {
        result_.textures.push_back(std::move(target_texture.texture));
    }
    
    calculate_bounding_boxes();
    add_edge_component();
}

void ts::game::Track_builder::add_tile_to_scene(const Tile_entry& tile_entry)
{
    const auto& placed_tile = tile_entry.placed_tile;
    const auto& target_texture = *tile_entry.target_texture;

    const auto* tile_def = placed_tile.tile_def;

    const auto& pattern_rect = tile_def->pattern_rect;
    const auto& image_rect = tile_def->image_rect;

    auto mapping_it = target_texture.tile_mapping.find(tile_def);
    if (mapping_it != target_texture.tile_mapping.end())
    {
        const auto& tile_mapping = mapping_it->second;
        const auto* texture = target_texture.texture.get();

        Track_component* component = nullptr;
        if (!result_.components.empty())
        {
            auto& last_component = result_.components.back();
            if (last_component.level == placed_tile.tile.level && last_component.texture == texture)
            {
                component = &last_component;
            }
        }

        if (component == nullptr)
        {
            result_.components.emplace_back();
            component = &result_.components.back();
            component->texture = texture;
            component->level = placed_tile.tile.level;
        }

        // Need to get the track position at the four corners of the source rect.
        sf::Vertex vertices[4];

        auto cos = std::cos(placed_tile.tile.rotation.radians());
        auto sin = std::sin(placed_tile.tile.rotation.radians());

        const auto& texture_rect = tile_mapping.target_rect;

        const auto texture_left = static_cast<float>(texture_rect.left);
        const auto texture_top = static_cast<float>(texture_rect.top);
        const auto texture_right = static_cast<float>(texture_rect.right());
        const auto texture_bottom = static_cast<float>(texture_rect.bottom());

        vertices[0].texCoords = sf::Vector2f(texture_left, texture_top);
        vertices[1].texCoords = sf::Vector2f(texture_left, texture_bottom);
        vertices[2].texCoords = sf::Vector2f(texture_right, texture_bottom);
        vertices[3].texCoords = sf::Vector2f(texture_right, texture_top);

        const auto center_x = pattern_rect.width * 0.5;
        const auto center_y = pattern_rect.height * 0.5;

        auto scale_x = 0.5;
        auto scale_y = 0.5;

        if (pattern_rect.width * 2 != image_rect.width) scale_x = static_cast<double>(pattern_rect.width) / image_rect.width;
        if (pattern_rect.height * 2 != image_rect.height) scale_y = static_cast<double>(pattern_rect.height) / image_rect.height;

        auto source_rect = tile_mapping.source_rect;

        auto source_left = static_cast<double>(source_rect.left) - image_rect.left;
        auto source_top = static_cast<double>(source_rect.top) - image_rect.top;

        auto source_right = source_left + source_rect.width;
        auto source_bottom = source_top + source_rect.height;

        // Position of coord [0, 0]:
        const auto point = transform_point({ 0.0 - center_x, 0.0 - center_y }, sin, cos);

        auto top_left = transform_point({ source_left * scale_x - center_x, source_top * scale_y - center_y }, sin, cos);
        auto bottom_left = transform_point({ source_left * scale_x - center_x, source_bottom * scale_y - center_y }, sin, cos);
        auto bottom_right = transform_point({ source_right * scale_x - center_x, source_bottom * scale_y - center_y }, sin, cos);
        auto top_right = transform_point({ source_right * scale_x - center_x, source_top * scale_y - center_y }, sin, cos);

        const auto tile_position = placed_tile.tile.position;

        top_left += tile_position;
        bottom_left += tile_position;
        bottom_right += tile_position;
        top_right += tile_position;

        vertices[0].position = { static_cast<float>(top_left.x), static_cast<float>(top_left.y) };
        vertices[1].position = { static_cast<float>(bottom_left.x), static_cast<float>(bottom_left.y) };
        vertices[2].position = { static_cast<float>(bottom_right.x), static_cast<float>(bottom_right.y) };
        vertices[3].position = { static_cast<float>(top_right.x), static_cast<float>(top_right.y) };

        component->vertices.push_back(vertices[0]);
        component->vertices.push_back(vertices[1]);
        component->vertices.push_back(vertices[2]);
        component->vertices.push_back(vertices[3]);
    }
}

void ts::game::Track_builder::add_tile(const resources::Placed_tile& placed_tile)
{
    auto target_texture = current_target_texture_;

    const auto& tile_mapping = target_texture->tile_mapping;
    const auto* tile_def = placed_tile.tile_def;

    auto mapping_it = tile_mapping.find(tile_def);
    if (mapping_it == tile_mapping.end())
    {
        const auto& image_file = tile_def->image_file();

        auto& tile_list = target_texture->tile_placement[image_file];

        auto image_rect = tile_def->image_rect;
        auto intersect_it = std::partition(tile_list.begin(), tile_list.end(),
                                           [image_rect](const impl::Tile_placement& tile_placement)
        {
            return !intersects(tile_placement.source_rect, image_rect) ||
                intersection(tile_placement.source_rect, image_rect) == image_rect;
        });
        
        for (auto it = intersect_it; it != tile_list.end(); ++it)
        {
            image_rect = combine(image_rect, it->source_rect);
        }

        Vector2i area_size(image_rect.width, image_rect.height);
        if (area_size.x > (target_texture->texture_size.x >> 1) ||
            area_size.y > (target_texture->texture_size.y >> 1))
        {
            add_large_tile(placed_tile, image_rect);
        }

        else 
        {
            auto allocation = allocate_tile_space(image_rect);
            auto allocated_space = allocation.second;

            if (allocation.first == target_texture)
            {
                // We haven't allocated a new texture, so we have to adjust all the 
                // intersecting tiles.
                for (auto it = intersect_it; it != tile_list.end(); ++it)
                {
                    target_texture->deallocate_tile_space(it->target_rect);
                }

                tile_list.erase(intersect_it, tile_list.end());
            }

            target_texture = allocation.first;

            impl::Tile_placement tile_placement;
            tile_placement.source_rect = image_rect;
            tile_placement.target_rect = allocated_space;
            target_texture->tile_placement[image_file].push_back(tile_placement);

            target_texture->tile_mapping[tile_def];
            add_tile_entry(placed_tile, target_texture);
        }
    }

    else
    {
        add_tile_entry(placed_tile, target_texture);
    }
}

void ts::game::Track_builder::add_tile_entry(const resources::Placed_tile& placed_tile, impl::Target_texture* target_texture)
{
    tile_entries_.push_back({ placed_tile, target_texture });
}

ts::game::impl::Target_texture* ts::game::Track_builder::create_new_target_texture()
{
    auto texture_size = std::min(sf::Texture::getMaximumSize(), 2048U);

    target_texture_list_.emplace_back();

    current_target_texture_ = &target_texture_list_.back();
    current_target_texture_->texture_size = Vector2i(texture_size, texture_size);
   
    return current_target_texture_;
}

void ts::game::Track_builder::calculate_bounding_boxes()
{
    auto& components = result_.components;

    for (auto& component : components) 
    {
        const auto& vertices = component.vertices;
        auto& bounding_box = component.bounding_box;

        std::size_t idx = 0, count = vertices.size();
        float min_x = vertices[idx].position.x, max_x = min_x;
        float min_y = vertices[idx].position.y, max_y = min_y;

        for (++idx; idx != count; ++idx) 
        {
            const auto& vertex = vertices[idx];
            auto x = vertex.position.x, y = vertex.position.y;

            if (x < min_x) min_x = x;
            if (x > max_x) max_x = x;
            if (y < min_y) min_y = y;
            if (y > max_y) max_y = y;
        }

        bounding_box.left = min_x;
        bounding_box.top = min_y;
        bounding_box.width = max_x - min_x;
        bounding_box.height = max_y - min_y;
    }
}

void ts::game::Track_builder::add_edge_component()
{
    Vector2i track_size = track_.size();

    double track_width = static_cast<double>(track_size.x);
    double track_height = static_cast<double>(track_size.y);

    double min_x = 0.0, min_y = 0.0;
    double max_x = track_width, max_y = track_height;

    std::uint32_t level = 0;

    for (const auto& component : result_.components)
    {
        const auto& bb = component.bounding_box;
        if (bb.left < min_x) min_x = bb.left;
        if (bb.top < min_y) min_y = bb.top;

        auto right = bb.right(), bottom = bb.bottom();
        if (right > max_x) max_x = right;
        if (bottom > max_y) max_y = bottom;

        if (component.level > level) level = component.level;
    }

    result_.components.emplace_back();
    auto& edge_component = result_.components.back();
    
    edge_component.level = level + 1;
    edge_component.bounding_box.left = min_x;
    edge_component.bounding_box.top = min_y;
    edge_component.bounding_box.width = max_x - min_x;
    edge_component.bounding_box.height = max_y - min_y;

    Vector2<double> positions[] =
    {
        { min_x, min_y }, { min_x, 0 }, { max_x, 0 }, { max_x, min_y },
        { min_x, max_y }, { min_x, track_height }, { max_x, track_height }, { max_x, max_y },
        { min_x, min_y }, { 0, min_y }, { 0, max_y }, { min_x, max_y },
        { max_x, min_y}, { max_x, max_y }, { track_width, max_y }, { track_width, min_y }
    };

    auto& vertices = edge_component.vertices;
    vertices.reserve(16);

    for (const auto& position : positions)
    {
        vertices.emplace_back(sf::Vector2f(static_cast<float>(position.x), static_cast<float>(position.y)), sf::Color::Black);
    }
}

ts::game::Track_scene ts::game::Track_builder::operator()(std::function<void()> step_operation)
{
    build_scene(step_operation);

    return std::move(result_);
}