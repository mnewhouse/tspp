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
#include "track.hpp"

#include "terrain_library.hpp"
#include "tile_library.hpp"
#include "track_handle.hpp"
#include "track_loader.hpp"

struct ts::resources::Track::Track_features
{
    void sort_tile_list();
    void create_default_start_points();
    void update_placed_tiles();

    Vector2u size_;
    std::size_t num_levels_ = 1;

    std::vector<Level_tile> tile_list_;
    std::vector<Placed_tile> placed_tiles_;
    std::vector<Start_point> start_points_;
    std::vector<Control_point> control_points_;

    Terrain_library terrain_library_;
    Tile_library tile_library_;

    bool dirty_tile_list_ = false;
    bool dirty_placed_tiles_ = false;
};

ts::resources::Track::Track()
: track_features_(std::make_unique<Track_features>())
{
}

ts::resources::Track::~Track()
{
}

ts::resources::Track::Track(const Track_handle& track_handle)
: Track()
{
    load_from_file(track_handle.path());
}

ts::resources::Track::Track(Track&& other)
: track_features_(std::move(other.track_features_))
{
}

ts::resources::Track& ts::resources::Track::operator=(Track&& other)
{
    track_features_ = std::move(other.track_features_);
    return *this;
}

void ts::resources::Track::load_from_file(const utf8_string& file_name)
{
    Track_loader track_loader;
    track_loader.load_from_file(file_name);
    *this = track_loader.get_result();
}

const ts::resources::Tile_library& ts::resources::Track::tile_library() const
{
    return track_features_->tile_library_;
}

const ts::resources::Terrain_library& ts::resources::Track::terrain_library() const
{
    return track_features_->terrain_library_;
}

const std::vector<ts::resources::Level_tile>& ts::resources::Track::tile_list() const
{
    if (track_features_->dirty_tile_list_)
    {
        track_features_->sort_tile_list();
    }

    return track_features_->tile_list_;
}

const std::vector<ts::resources::Start_point>& ts::resources::Track::start_points() const
{
    if (track_features_->start_points_.empty())
    {
        track_features_->create_default_start_points();
    }

    return track_features_->start_points_;
}

const std::vector<ts::resources::Control_point>& ts::resources::Track::control_points() const
{
    return track_features_->control_points_;
}

ts::Vector2u ts::resources::Track::size() const
{
    return track_features_->size_;
}

void ts::resources::Track::set_size(Vector2u size, std::size_t num_levels)
{
    track_features_->size_ = size;
    track_features_->num_levels_ = num_levels;
}

std::size_t ts::resources::Track::num_levels() const
{
    return track_features_->num_levels_;
}

void ts::resources::Track::place_tile(const Level_tile& level_tile)
{
    track_features_->tile_list_.push_back(level_tile);

    track_features_->dirty_tile_list_ = true;
    track_features_->dirty_placed_tiles_ = true;
}

const std::vector<ts::resources::Placed_tile>& ts::resources::Track::placed_tiles() const
{
    if (track_features_->dirty_placed_tiles_)
    {
        track_features_->update_placed_tiles();
    }

    return track_features_->placed_tiles_;
}

std::size_t ts::resources::Track::placed_tile_count() const
{
    return placed_tiles().size();
}

void ts::resources::Track::define_tile(const Tile_definition& tile_def)
{
    track_features_->tile_library_.define_tile(tile_def);
}

void ts::resources::Track::define_tile_group(const Tile_group_definition& tile_group_def)
{
    track_features_->tile_library_.define_tile_group(tile_group_def);
}

void ts::resources::Track::define_terrain(const Terrain_definition& terrain_def)
{
    track_features_->terrain_library_.define_terrain(terrain_def);
}

void ts::resources::Track::define_sub_terrain(const Sub_terrain& sub_terrain)
{
    track_features_->terrain_library_.define_sub_terrain(sub_terrain);
}

void ts::resources::Track::add_control_point(const Control_point& control_point)
{
    track_features_->control_points_.push_back(control_point);
}

void ts::resources::Track::add_start_point(const Start_point& start_point)
{
    track_features_->start_points_.push_back(start_point);
}

void ts::resources::Track::Track_features::create_default_start_points()
{
    start_points_.reserve(20);

    Vector2<double> start(size_.x * 0.5, 0.0);
    Vector2<double> end(size_.x * 0.5, size_.y);

    if (!control_points_.empty())
    {
        const auto& control_point = control_points_.front();
        start = control_point.start;
        end = control_point.end;
    }

    auto cp_direction = normalize(end - start), grid_direction = cp_direction;

    std::swap(grid_direction.x, grid_direction.y);
    grid_direction.x = -grid_direction.x;

    auto center = start + (end - start) * 0.5;
    auto left_column_start = center - 12.0 * cp_direction + grid_direction * 15.0;
    auto right_column_start = center + 12.0 * cp_direction + grid_direction * 27.0;

    Start_point start_point;
    start_point.level = 0;
    start_point.rotation = Rotation<double>::radians(std::atan2(-grid_direction.x, -grid_direction.y));

    Vector2<double> offset;

    for (std::size_t i = 0; i != 10; ++i)
    {
        start_point.position = left_column_start + offset;
        start_points_.push_back(start_point);

        start_point.position = right_column_start + offset;
        start_points_.push_back(start_point);

        offset += grid_direction * 24.0;
    }
}

void ts::resources::Track::Track_features::update_placed_tiles()
{
    placed_tiles_.clear();
    placed_tiles_.reserve(tile_list_.size());

    for (const auto& tile : tile_list_)
    {
        const auto* tile_group = tile_library_.tile_group(tile.id);
        if (!tile_group) continue;

        for (const auto& sub_tile : tile_group->sub_tiles())
        {
            const auto* tile_def = tile_library_.tile(sub_tile.id);
            if (!tile_def) continue;

            auto sub_tile_offset = transform_point(sub_tile.position, tile.rotation);

            Placed_tile placed_tile;
            placed_tile.tile_def = tile_def;
            placed_tile.tile.id = sub_tile.id;
            placed_tile.tile.level = sub_tile.level;
            placed_tile.tile.position = tile.position + sub_tile_offset;
            placed_tile.tile.rotation = tile.rotation + sub_tile.rotation;

            placed_tiles_.push_back(placed_tile);
        }
    }

    dirty_placed_tiles_ = false;
}

void ts::resources::Track::Track_features::sort_tile_list()
{
    std::sort(tile_list_.begin(), tile_list_.end(), 
              [](const Level_tile& a, const Level_tile& b)
    {
        return a.level < b.level;
    });

    dirty_tile_list_ = false;
}