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

#ifndef GAME_TRACK_BUILDER_HPP
#define GAME_TRACK_BUILDER_HPP

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "graphics/image_loader.hpp"

#include "resources/tile.hpp"

namespace ts
{
    namespace resources
    {
        class Track;
    }

    namespace graphics
    {
        class Texture;
    }

    namespace game
    {

        namespace impl
        {
            struct Tile_mapping
            {
                const sf::Image* image;
                Int_rect source_rect;
                Int_rect target_rect;
            };


            struct Texture_row
            {                
                Int_rect area;
                std::int32_t free_space = 0;
            };

            struct Tile_placement
            {
                Int_rect source_rect;
                Int_rect target_rect;
            };

            struct Target_texture
            {
                Vector2i texture_size;

                std::unique_ptr<const sf::Texture> texture;
                std::vector<Texture_row> texture_rows;

                std::unordered_map<const resources::Tile_definition*, Tile_mapping> tile_mapping;
                std::unordered_map<utf8_string, std::vector<Tile_placement>> tile_placement;

                std::int32_t row_start = 0;

                Int_rect allocate_tile_space(Vector2i size);
                void deallocate_tile_space(Int_rect bounding_box);
            };
        }
        struct Track_component
        {
            Track_component() = default;
            Track_component(std::vector<sf::Vertex> vertices, const sf::Texture* texture, std::uint32_t level);

            std::vector<sf::Vertex> vertices;
            const sf::Texture* texture = nullptr;
            Rect<double> bounding_box;
            std::uint32_t level = 0;
        };

        struct Track_scene
        {
            Track_scene(Vector2u track_size);

            Track_scene(Track_scene&& other);
            Track_scene& operator=(Track_scene&& rhs);

            std::vector<Track_component> components;
            std::vector<std::unique_ptr<const sf::Texture>> textures;

            Vector2u track_size;
        };

        class Track_builder
        {
        public:
            Track_builder(const resources::Track& track);

            Track_scene operator()(std::function<void()> step_operation = {});

            void preload_image(const utf8_string& path);

            std::size_t step_count() const;

        private:
            struct Tile_entry
            {
                resources::Placed_tile placed_tile;
                impl::Target_texture* target_texture;
            };

            void gather_tile_info();
            void generate_textures();
            void build_scene(std::function<void()> step_operation);

            void add_tile(const resources::Placed_tile& placed_tile);
            void add_tile_to_scene(const Tile_entry& tile_entry);

            void add_tile_entry(const resources::Placed_tile& placed_tile, impl::Target_texture* target_texture);
            

            const utf8_string& resolve_path(const utf8_string& path);

            void add_large_tile(const resources::Placed_tile& placed_tile, Int_rect image_rect);
            std::pair<impl::Target_texture*, Int_rect> allocate_tile_space(Int_rect image_rect);            

            impl::Target_texture* create_new_target_texture();

            void calculate_bounding_boxes();
            void add_edge_component();

            void build_tile_mapping(impl::Target_texture& target_texture);
            void generate_texture(impl::Target_texture& target_texture);

            Track_scene result_;
            graphics::Image_loader image_loader_;
            std::map<utf8_string, utf8_string> precomputed_paths_;


            std::vector<Tile_entry> tile_entries_;
            std::unordered_map<const resources::Tile_definition*, std::vector<impl::Target_texture*>> large_tile_mapping_;

            sf::Image image_buffer_;
            
            const resources::Track& track_;

            std::list<impl::Target_texture> target_texture_list_;
            impl::Target_texture* current_target_texture_ = nullptr;

            
        };
    }
}

#endif