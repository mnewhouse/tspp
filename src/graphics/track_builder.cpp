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

#include "track_builder.hpp"
#include "texture_loader.hpp"
#include "texture.hpp"

#include "resources/track.hpp"

#include <array>
#include <algorithm>

#include <SFML/Graphics/Transformable.hpp>

namespace {

bool rect_overlap (const sf::Vertex* a, const sf::Vertex* b)
{
    auto bounding_box = [](const sf::Vertex* quad)
    {
        auto x = {
            quad[0].position.x, quad[1].position.x,
            quad[2].position.x, quad[3].position.x
        };

        auto y = {
            quad[0].position.y, quad[1].position.y,
            quad[2].position.y, quad[3].position.y
        };

        auto min_x = std::min(x);
        auto min_y = std::min(y);
        auto max_x = std::max(x);
        auto max_y = std::max(y);

        return ts::Rect<float>(min_x, min_y, max_x - min_x, max_y - min_y);
    };

    return intersects(bounding_box(a), bounding_box(b));
}

bool quad_overlap(const sf::Vertex* a, const sf::Vertex* b)
{
    if (!rect_overlap(a, b)) return false;

    auto project = [](sf::Vector2f point, sf::Vector2f normal)
    {
        return normal.x * point.x + normal.y * point.y;
    };

    for (unsigned int i = 0; i != 2; ++i) {
        for (unsigned int idx = 0; idx != 4;) {
            const auto& p1 = a[idx].position;
            const auto& p2 = a[++idx % 4].position;

            sf::Vector2f normal(p2.y - p1.y, p2.x - p1.x);

            float min_a = project(a[0].position, normal), max_a = min_a;
            for (unsigned int p = 1; p != 4; ++p) {
                auto projection = project(a[p].position, normal);
                if (projection < min_a) min_a = projection;
                if (projection > max_a) max_a = projection;
            }

            float min_b = project(b[0].position, normal), max_b = min_b;
            for (unsigned int p = 1; p != 4; ++p) {
                auto projection = project(b[p].position, normal);
                if (projection < min_b) min_b = projection;
                if (projection > max_b) max_b = projection;
            }

            if (max_a < min_b || max_b < min_a) {
                return false;
            }
        }

        std::swap(a, b);
    }

    return true;
}

bool component_overlap(const std::vector<sf::Vertex>& vertices, const sf::Vertex* quad)
{
    const auto count = vertices.size();

    for (std::size_t idx = 0; idx != count; idx += 4) {
        if (quad_overlap(&vertices[idx], quad)) {
            return true;
        }
    }

    return false;
};

}


namespace ts
{

namespace graphics
{

void add_sub_tile(Track_builder_result& result, Texture_loader& texture_loader,
    const resources::Track& track, const resources::Tile_definition& tile_def,
    const resources::Level_tile& tile, const resources::Level_tile& sub_tile)
{
    auto& textures = result.textures;
    auto& components = result.components;

    auto image_path = track.find_include_path(tile_def.image_file());
    auto texture_handle = texture_loader.load_from_file(image_path);

    const auto& image_rect = tile_def.image_rect;
    const auto& pattern_rect = tile_def.pattern_rect;

    auto make_vertices = [&](const sf::Texture& texture, const sf::IntRect& rect)
    {
        sf::Vector2f tile_position(float(tile.position.x), float(tile.position.y));
        sf::Vector2f sub_tile_position(float(sub_tile.position.x), float(sub_tile.position.y));

        auto pattern_width = float(pattern_rect.width);
        auto pattern_height = float(pattern_rect.height);

        auto image_left = float(rect.left) + 0.1f;
        auto image_top = float(rect.top) + 0.1f;
        auto image_right = float(rect.left + rect.width) - 0.1f;
        auto image_bottom = float(rect.top + rect.height) - 0.1f;


        sf::Transformable transformable;
        transformable.rotate(float(tile.rotation.degrees()));
        auto transform = transformable.getTransform();


        sf::Vertex vertex;
        sf::Vector2f sub_tile_center = transform.transformPoint(sub_tile_position);
        sub_tile_center += tile_position;

        transformable.setOrigin(pattern_width * 0.5f, pattern_height * 0.5f);
        transformable.rotate(float(sub_tile.rotation.degrees()));
        transform = transformable.getTransform();

        std::array<sf::Vertex, 4> vertex_buffer;

        vertex_buffer[0].position = transform.transformPoint(0, 0) + sub_tile_center;
        vertex_buffer[0].texCoords.x = image_left;
        vertex_buffer[0].texCoords.y = image_top;

        vertex_buffer[1].position = transform.transformPoint(0, pattern_height) + sub_tile_center;
        vertex_buffer[1].texCoords.x = image_left;
        vertex_buffer[1].texCoords.y = image_bottom;

        vertex_buffer[2].position = transform.transformPoint(pattern_width, pattern_height) + sub_tile_center;
        vertex_buffer[2].texCoords.x = image_right;
        vertex_buffer[2].texCoords.y = image_bottom;

        vertex_buffer[3].position = transform.transformPoint(pattern_width, 0) + sub_tile_center;
        vertex_buffer[3].texCoords.x = image_right;
        vertex_buffer[3].texCoords.y = image_top;

        auto find_suitable_component = [&]()
        {
            auto overlap = [&](const Track_component& component)
            {
                return component_overlap(component.vertices, &vertex_buffer[0]);
            };

            auto matching_component = [&texture, &sub_tile](const Track_component& component)
            {
                return &texture == component.texture && component.level == sub_tile.level;
            };

            // Get the last matching component
            auto component_it = std::find_if(components.rbegin(), components.rend(), matching_component);

            // Then, check if any "later" components would overlap with the one we found
            auto it = std::find_if(components.rbegin(), component_it, overlap);

            if (it == component_it) {
                // Nothing overlaps, we're good to go
                return it;
            }

            // Overlap, we probably need to insert a new component at the end
            return components.rend();
        };

        auto component_it = find_suitable_component();

        // Check if there's a component to be added to
        if (component_it == components.rend()) {
            // Create new component
            std::vector<sf::Vertex> vertices(vertex_buffer.begin(), vertex_buffer.end());
            components.emplace_back(std::move(vertices), &texture, sub_tile.level);
        }

        else {
            auto& vertices = component_it->vertices;
            vertices.insert(vertices.end(), vertex_buffer.begin(), vertex_buffer.end());
        }
    };

    texture_handle->perform(make_vertices, image_rect);
    textures.insert(texture_handle);
}

void optimize_component_list(std::vector<Track_component>& components)
{
    using ts::graphics::Track_component;

    for (auto a_it = components.begin(); a_it != components.end(); ++a_it)
    {
        auto overlap_it = std::find_if(std::next(a_it), components.end(),
            [a_it](const Track_component& component)
        {
            for (std::size_t idx = 0; idx != component.vertices.size(); idx += 4) {
                if (component_overlap(a_it->vertices, &component.vertices[idx])) return true;
            }

            return false;
        });

        for (auto b_it = std::next(a_it); b_it != overlap_it; ++b_it) {
            if (b_it->texture == a_it->texture) continue;

            auto a2_it = std::find_if(std::next(b_it), overlap_it,
                [a_it](const Track_component& component)
            {
                return component.texture == a_it->texture;
            });

            auto b2_it = std::find_if(a2_it, overlap_it,
                [b_it](const Track_component& component)
            {
                return component.texture == b_it->texture;
            });

            if (a2_it != overlap_it && b2_it != overlap_it) {
                auto b_overlap = std::find_if(std::next(b_it), b2_it,
                    [b_it](const Track_component& component)
                {
                    for (std::size_t idx = 0; idx != component.vertices.size(); idx += 4) {
                        if (component_overlap(b_it->vertices, &component.vertices[idx])) return true;
                    }

                    return false;
                });

                if (b_overlap == b2_it) {
                    a_it->vertices.insert(a_it->vertices.end(), a2_it->vertices.begin(), a2_it->vertices.end());
                    b_it->vertices.insert(b_it->vertices.end(), b2_it->vertices.begin(), b2_it->vertices.end());

                    *a2_it = std::move(*a_it);
                    *b2_it = std::move(*b_it);

                    a_it->vertices.clear();
                    b_it->vertices.clear();
                }
            }
        }
    }
}


void calculate_bounding_boxes(std::vector<Track_component>& components)
{
    for (auto& component : components) {
        const auto& vertices = component.vertices;

        std::size_t idx = 0, count = vertices.size();
        float min_x = vertices[idx].position.x, max_x = min_x;
        float min_y = vertices[idx].position.y, max_y = min_y;

        for (++idx; idx != count; ++idx) {
            const auto& vertex = vertices[idx];
            auto x = vertex.position.x, y = vertex.position.y;

            if (x < min_x) min_x = x;
            if (x > max_x) max_x = x;
            if (y < min_y) min_y = y;
            if (y > max_y) max_y = y;
        }

        auto& bounding_box = component.bounding_box;
        bounding_box.left = min_x;
        bounding_box.top = min_y;
        bounding_box.width = max_x - min_x;
        bounding_box.height = max_y - min_y;
    }
}

}

}

ts::graphics::Track_builder_result ts::graphics::build_track(const resources::Track& track)
{
    graphics::Texture_loader texture_loader;
    const auto& tile_library = track.tile_library();

    Track_builder_result result;

    auto& components = result.components;

    for (const auto& tile : track.tile_list()) {
        const auto* tile_group = tile_library.tile_group(tile.id);
        if (!tile_group) continue;

        for (const auto& sub_tile : tile_group->sub_tiles()) {
            const auto* tile_def = tile_library.tile(sub_tile.id);
            if (!tile_def) continue;

            add_sub_tile(result, texture_loader, track, *tile_def, tile, sub_tile);
        }
    }

    optimize_component_list(components);

    components.erase(std::remove_if(components.begin(), components.end(),
        [](const Track_component& component)
    {
        return component.vertices.empty();
    }), components.end());


    // Sort components by level
    std::stable_sort(components.begin(), components.end(), 
              [](const Track_component& a, const Track_component& b)
    {
        return a.level < b.level;
    });

    calculate_bounding_boxes(components);

    return result;
}
