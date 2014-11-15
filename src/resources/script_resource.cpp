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
#include "script_resource.hpp"
#include "car_store.hpp"

namespace ts
{
    namespace resources
    {
        utf8_string resource_car_directory(const utf8_string& resource_directory);
    }
}

ts::utf8_string ts::resources::resource_car_directory(const utf8_string& resource_root_directory)
{
    boost::filesystem::path path(resource_root_directory.string());
    path /= "cars";
    return path.string();
}

struct ts::resources::Script_resource::Resources
{
    Resources(const utf8_string& root_directory);

    Car_store car_store;  
};

ts::resources::Script_resource::Resources::Resources(const utf8_string& root_directory)
: car_store(resource_car_directory(root_directory))
{
}

ts::resources::Script_resource::Script_resource(utf8_string root_directory, utf8_string name)
: resource_name_(std::move(name)),
  root_directory_(std::move(root_directory)),
  resources_(std::make_unique<Resources>(root_directory_))
{
    load_resource_config();
}

ts::resources::Script_resource::~Script_resource()
{
}

const ts::utf8_string& ts::resources::Script_resource::name() const
{
    return resource_name_;
}

const std::vector<ts::utf8_string>& ts::resources::Script_resource::client_scripts() const
{
    return client_script_files_;
}

const std::vector<ts::utf8_string>& ts::resources::Script_resource::server_scripts() const
{
    return server_script_files_;
}

const std::vector<ts::utf8_string>& ts::resources::Script_resource::cup_scripts() const
{
    return cup_script_files_;
}

const ts::resources::Car_store* ts::resources::Script_resource::car_store() const
{
    return &resources_->car_store;
}

ts::resources::Resource_config_exception::Resource_config_exception(const utf8_string& resource_name)
: runtime_error("could not load '" + std::string(script_config_file_name) + "' in resource '" + resource_name.string() + "'")
{
}

void ts::resources::Script_resource::load_resource_config()
{
    boost::filesystem::path config_path = root_directory_.string();
    config_path /= script_config_file_name;

    boost::filesystem::ifstream stream(config_path, std::istream::in);
    if (!stream)
    {
        throw Resource_config_exception(resource_name_);
    }

    load_resource_config(stream);
}

void ts::resources::Script_resource::load_resource_config(std::istream& stream)
{
    boost::filesystem::path root_path = root_directory_.string();

    for (std::string line, directive; std::getline(stream, line) && directive != "end";)
    {
        std::istringstream line_stream(line);
        read_directive(line_stream, directive);

        if (directive == "clientscript")
        {
            std::string script_path;
            line_stream >> std::ws;

            if (std::getline(line_stream, script_path))
            {
                auto path = root_path / script_path;

                client_script_files_.push_back(path.string());
            }
        }

        else if (directive == "serverscript")
        {
            std::string script_path;
            line_stream >> std::ws;

            if (std::getline(line_stream, script_path))
            {
                auto path = root_path / script_path;

                server_script_files_.push_back(path.string());
            }
        }

        else if (directive == "cupscript")
        {
            std::string script_path;
            line_stream >> std::ws;

            if (std::getline(line_stream, script_path))
            {
                auto path = root_path / script_path;

                cup_script_files_.push_back(path.string());
            }
        }
    }
}