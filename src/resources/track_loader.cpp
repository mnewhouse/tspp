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
#include "track_loader.hpp"
#include "track.hpp"
#include "track_handle.hpp"

#include "include_path.hpp"
#include "terrain_definition.hpp"
#include "tile.hpp"


struct ts::resources::Track_loader::Impl
{
    void load_from_file(const utf8_string& file_name);
    void load_from_stream(std::istream& stream, utf8_string working_directory);

    void include(const utf8_string& file_name);
    void include(std::istream& stream);

    void process_tile_group_definition(std::istream& stream, Tile_id group_id, std::size_t group_size);
    void process_tile_definition(std::istream& stream, const utf8_string& pattern_name, const utf8_string& image_name);
    void process_terrain_definition(std::istream& stream);
    void process_control_points(std::istream& stream, std::size_t num_points);
    void process_start_points(std::istream& stream, std::size_t num_points);

    utf8_string resolve_asset_path(const utf8_string& file_name);
    void add_asset(utf8_string file_path);

    std::unordered_set<utf8_string> included_files_;
    std::vector<utf8_string> assets_;
    utf8_string working_directory_;

    Track track_;

    std::istringstream line_stream_;
    std::string directive_;
    std::string line_;
};


ts::resources::Broken_track_exception::Broken_track_exception(utf8_string missing_file)
: std::logic_error("broken track (missing file '" + missing_file.string() + "')"),
  missing_file_(std::move(missing_file))
{
}

ts::utf8_string ts::resources::Track_loader::Impl::resolve_asset_path(const utf8_string& file_name)
{
    auto include_directory = find_include_directory(file_name, { working_directory_, config::data_directory });

    boost::filesystem::path path = include_directory.string();
    path /= file_name.string();

    return path.string();
}

void ts::resources::Track_loader::Impl::add_asset(utf8_string file_path)
{
    if (std::find(assets_.begin(), assets_.end(), file_path) == assets_.end())
    {
        assets_.push_back(std::move(file_path));
    }
}

void ts::resources::Track_loader::Impl::load_from_file(const utf8_string& file_name)
{
    track_ = Track();

    working_directory_ = boost::filesystem::path(file_name.string()).parent_path().string();

    include(file_name);
}

void ts::resources::Track_loader::Impl::load_from_stream(std::istream& stream, utf8_string working_directory)
{
    working_directory_ = std::move(working_directory);

    include(stream);
}

void ts::resources::Track_loader::Impl::include(const utf8_string& file_name)
{
    // Test if the file has not been included before
    if (included_files_.find(file_name) == included_files_.end())
    {
        auto include_path = resolve_asset_path(file_name);        

        boost::filesystem::ifstream stream(include_path.string(), std::istream::in);
        if (!stream)
        {
            throw Broken_track_exception(file_name);
        }

        add_asset(include_path);

        included_files_.insert(std::move(include_path));
        include(stream);
    }
}

void ts::resources::Track_loader::Impl::include(std::istream& stream)
{
    std::istringstream line_stream;

    std::string params[2];
    for (std::string line, directive; directive != "end" && std::getline(stream, line); )
    {
        line_stream.clear();
        line_stream.str(line);

        read_directive(line_stream, directive);

        if (directive == "a")
        {
            Tile tile;
            if (line_stream >> tile)
            {
                track_.place_tile(tile);
            }
        }
        
        else if (directive == "tiledefinition" && line_stream >> params[0] >> params[1])
        {
            process_tile_definition(stream, params[0], params[1]);
        }

        else if (directive == "terrain")
        {
            process_terrain_definition(stream);
        }
        
        else if (directive == "subterrain")
        {
            Sub_terrain sub_terrain;
            if (line_stream >> sub_terrain)
            {
                track_.define_sub_terrain(sub_terrain);
            }
        }

        else if (directive == "tilegroup" || directive == "norottilegroup")
        {
            std::size_t group_size;
            Tile_id group_id;
            if (line_stream >> group_id >> group_size)
            {
                process_tile_group_definition(stream, group_id, group_size);
            }
        }

        else if (directive == "leveltile")
        {
            Level_tile level_tile;
            if (line_stream >> level_tile)
            {
                track_.place_tile(level_tile);
            }            
        }

        else if (directive == "include")
        {
            auto& include_path = params[0];

            if (std::getline(line_stream, include_path))
            {                
                boost::trim(include_path);

                include(include_path);
            }
        }

        else if (directive == "size")
        {
            Vector2u size;
            auto line_pos = line_stream.tellg();            

            if (line_stream >> params[0] && params[0] == "td")
            {
                std::size_t num_levels;
                if (line_stream >> num_levels >> size)
                {
                    track_.set_size(size, num_levels);
                }
            }

            else
            {
                line_stream.seekg(line_pos);
                if (line_stream >> size)
                {
                    track_.set_size(size, 1);
                }
            }
        }

        else if (directive == "controlpoints")
        {
            std::size_t num_points;
            if (line_stream >> num_points)
            {
                process_control_points(stream, num_points);
            }
        }

        else if (directive == "startpoints")
        {
            std::size_t num_points;
            if (line_stream >> num_points)
            {
                process_start_points(stream, num_points);
            }
        }

        else if (directive == "pattern")
        {
            auto& pattern_file = params[0];

            if (std::getline(line_stream, pattern_file))
            {
                boost::trim(pattern_file);

                auto pattern_path = resolve_asset_path(pattern_file);
                add_asset(std::move(pattern_path));
            }
        }
    }
}

void ts::resources::Track_loader::Impl::process_tile_definition(std::istream& stream, const utf8_string& pattern_file, const utf8_string& image_file)
{
    auto pattern_path = resolve_asset_path(pattern_file);
    auto image_path = resolve_asset_path(image_file);

    if (!pattern_path.empty() && !image_path.empty())
    {
        Tile_definition tile_def(pattern_path, image_path);

        add_asset(std::move(pattern_path));
        add_asset(std::move(image_path));    
        
        for (directive_.clear(); directive_ != "end" && std::getline(stream, line_); )
        {
            line_stream_.clear();
            line_stream_.str(line_);

            read_directive(line_stream_, directive_);

            if ((directive_ == "tile" || directive_ == "norottile") && line_stream_ >> tile_def)
            {
                track_.define_tile(tile_def);
            }
        }
    }
}

void ts::resources::Track_loader::Impl::process_tile_group_definition(std::istream& stream, Tile_id group_id, std::size_t group_size)
{
    Tile_group_definition tile_group(group_id, group_size);
    for (directive_.clear(); directive_ != "end" && std::getline(stream, line_); )
    {
        line_stream_.clear();
        line_stream_.str(line_);

        read_directive(line_stream_, directive_);

        if (directive_ == "a")
        {
            Tile tile;
            if (line_stream_ >> tile)
            {
                tile_group.add_sub_tile(tile);
            }
        }

        else if (directive_ == "leveltile")
        {
            Level_tile tile;
            if (line_stream_ >> tile)
            {
                tile_group.add_sub_tile(tile);
            }
        }
    }

    track_.define_tile_group(tile_group);
}

void ts::resources::Track_loader::Impl::process_terrain_definition(std::istream& stream)
{
    Terrain_definition terrain_def;
    if (stream >> terrain_def)
    {
        track_.define_terrain(terrain_def);
    }
}

void ts::resources::Track_loader::Impl::process_control_points(std::istream& stream, std::size_t num_points)
{
    for (directive_.clear(); directive_ != "end" && std::getline(stream, line_); )
    {
        line_stream_.clear();
        line_stream_.str(line_);

        read_directive(line_stream_, directive_);

        if (directive_ == "point")
        {
            Vector2i point;
            std::int32_t length;
            std::int32_t direction;
            if (line_stream_ >> point >> length >> direction)
            {
                auto cp_end = point;
                if (direction == 0) cp_end.y += length;
                else cp_end.x += length;

                Control_point control_point;
                control_point.type = Control_point::Type::Line;
                control_point.start = point;
                control_point.end = cp_end;

                track_.add_control_point(control_point);
            }
        }
    }
}

void ts::resources::Track_loader::Impl::process_start_points(std::istream& stream, std::size_t num_points)
{
    Start_point start_point;

    for (directive_.clear(); directive_ != "end" && std::getline(stream, line_); )
    {
        line_stream_.clear();
        line_stream_.str(line_);

        read_directive(line_stream_, directive_);

        if (line_stream_ >> start_point.position >> start_point.rotation)
        {
            if (!(line_stream_ >> start_point.level)) start_point.level = 0;

            track_.add_start_point(start_point);
        }
    }
}

ts::resources::Track_loader::Track_loader()
: impl_(std::make_unique<Impl>())
{
}

ts::resources::Track_loader::~Track_loader()
{
}

void ts::resources::Track_loader::load_from_file(const utf8_string& file_name)
{
    impl_->load_from_file(file_name);
}

void ts::resources::Track_loader::load_from_handle(const resources::Track_handle& track_handle)
{
    impl_->load_from_file(track_handle.path());
}

void ts::resources::Track_loader::load_from_stream(std::istream& stream, utf8_string working_directory)
{
    impl_->load_from_stream(stream, std::move(working_directory));
}

ts::resources::Track ts::resources::Track_loader::get_result()
{
    return std::move(impl_->track_);
}

const std::vector<ts::utf8_string>& ts::resources::Track_loader::assets() const
{
    return impl_->assets_;
}
