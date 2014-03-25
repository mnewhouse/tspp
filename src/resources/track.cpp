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

#include "track.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "core/config.hpp"
#include "core/directive_reader.hpp"

#include "terrain_definition.hpp"
#include "track_handle.hpp"

#include "resources/include_path.hpp"

ts::resources::Track::Track()
  : num_levels_(0)
{
}

ts::resources::Track::Track(const Track_handle& track_handle)
  : num_levels_(0)
{
	load_from_file(track_handle.path());
}

void ts::resources::Track::load_from_file(const std::string& file_name)
{
	boost::filesystem::path path(file_name);
	track_directory_ = path.remove_filename().string();

    include(file_name);
}

void ts::resources::Track::include(const std::string& file_name)
{
	std::ifstream stream(file_name, std::ios::in);

	include(stream);
}

void ts::resources::Track::include(const std::string& file_name, std::size_t recursion_depth)
{
	auto include_path = find_include_path(file_name);

    std::ifstream stream(include_path, std::ios::in);
	if (!stream.is_open()) {
		throw std::runtime_error("broken track");
	}

	include(stream, recursion_depth);
}

const ts::resources::Tile_library& ts::resources::Track::tile_library() const
{
    return tile_lib_;
}

const ts::resources::Terrain_library& ts::resources::Track::terrain_library() const
{
    return terrain_lib_;
}

const std::vector<ts::resources::Level_tile>& ts::resources::Track::tile_list() const
{
	return tile_list_;
}

const std::vector<ts::resources::Track::Start_point>& ts::resources::Track::start_points() const
{
    return start_points_;
}

ts::Vector2u ts::resources::Track::size() const
{
    return track_size_;
}

std::size_t ts::resources::Track::num_levels() const
{
    return num_levels_;
}

template <typename Func>
bool read_placed_tile(std::istream& stream, const std::string& directive, Func process_tile)
{
    using namespace ts::resources;

    if (directive == "a") {
        Placed_tile tile;
        if (stream >> tile) {
            process_tile(tile);
            return true;
        }
    }

    else if (directive == "leveltile") {
        Level_tile tile;
        if (stream >> tile) {
            process_tile(tile);
            return true;
        }
    }

    return false;
}

void ts::resources::Track::include(std::istream& stream, std::size_t recursion_depth)
{
    for (std::string line, directive; directive != "end" && std::getline(stream, line); ) {
        boost::trim(line);
        std::istringstream line_stream(line);

        if (!read_directive(line_stream, directive)) 
            continue;

        if (read_placed_tile(line_stream, directive, [this](const Level_tile& tile) { add_tile(tile); })) 
            continue;

        if (directive == "tiledefinition") {
            std::string pattern_name, image_name;
            if (line_stream >> pattern_name >> image_name) {
                parse_tile_definition(stream, pattern_name, image_name);
            }
        }

        else if (directive == "include") {
            std::string file_name;

            if (std::getline(line_stream, file_name)) {
				boost::trim(file_name);

                include(file_name, recursion_depth + 1);
            }
        }

        else if (directive == "tilegroup" || directive == "norottilegroup") {
            std::size_t size;
            Tile_id id;
            if (line_stream >> id >> size) {
                parse_tile_group_definition(stream, id, size);
            }
        }

		else if (directive == "controlpoints") {
			std::size_t count;
			if (line_stream >> count) {
				parse_control_points(stream, count);
			}
		}

		else if (directive == "startpoints") {
			std::size_t count;
			if (line_stream >> count) {
				parse_start_points(stream, count);
			}
		}

		else if (directive == "terrain") {
			std::string terrain_name;
			if (std::getline(line_stream, terrain_name)) {
				parse_terrain(stream, terrain_name);
			}
		}

        else if (directive == "subterrain") {
            int id, sub_id, level_start, level_count;
            if (line_stream >> id >> sub_id >> level_start >> level_count) {
                terrain_lib_.define_sub_terrain(id, sub_id, level_start, level_count);
            }
        }

        else if (directive == "size") {            
            Vector2u size;
            if (line_stream >> size) {
                track_size_ = size;
            }

            else {
                line_stream.clear();

                std::size_t num_levels;
                std::string td;

                if (line_stream >> td && td == "td" &&
                    line_stream >> num_levels >> size) 
                {
                    track_size_ = size;
                    num_levels_ = num_levels;
                }
            }
        }
    }
}


void ts::resources::Track::parse_start_points(std::istream& stream, std::size_t count)
{
    for (std::string line, directive; directive != "end" && std::getline(stream, line);) {
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;

        Start_point start_point;
        if (line_stream >> start_point.position >> start_point.rotation)
        {
            if (!(line_stream >> start_point.level)) start_point.level = 0;

            start_points_.push_back(start_point);
        }
    }
}

void ts::resources::Track::parse_control_points(std::istream& stream, std::size_t count)
{
    for (std::string line, directive; directive != "end" && std::getline(stream, line);) {
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;
    }
}

void ts::resources::Track::parse_terrain(std::istream& stream, const std::string& terrain_name)
{
    Terrain_definition terrain_def;
    if (stream >> terrain_def) {
        terrain_lib_.define_terrain(terrain_def);
    }
}

void ts::resources::Track::parse_tile_definition
    (std::istream& stream, const std::string& pattern_file, const std::string& image_file)
{
    for (std::string line, directive; directive != "end" && std::getline(stream, line); ) {
        boost::trim(line);
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;

        if (directive == "tile" || directive == "norottile") {
            Tile_definition tile_def(pattern_file, image_file);
            if (line_stream >> tile_def) {
                tile_lib_.define_tile(tile_def);
            }            
        }
    }
}

void ts::resources::Track::parse_tile_group_definition(std::istream& stream, Tile_id id, std::size_t size)
{
    Tile_group_definition tile_group(id, size);
    auto add_sub_tile = [&](const Level_tile& tile) { tile_group.add_sub_tile(tile); };

    for (std::string line, directive; directive != "end" && std::getline(stream, line); )
    {
        std::istringstream line_stream(line);

        if (read_directive(line_stream, directive)) {
            read_placed_tile(line_stream, directive, add_sub_tile);
        }
    }

    tile_lib_.define_tile_group(tile_group);
}

void ts::resources::Track::add_tile(Level_tile level_tile)
{
    auto it = std::upper_bound(tile_list_.begin(), tile_list_.end(), level_tile,
        [](const Level_tile& a, const Level_tile& b) {
            return a.level < b.level;
        }
    );

    tile_list_.insert(it, level_tile);
}

std::string ts::resources::Track::find_include_path(const std::string& file_name) const
{
    return resources::find_include_path(file_name, { track_directory_, config::data_directory });
}