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

#include "car_store.hpp"
#include "car_definition.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>

#include "pattern.hpp"

ts::resources::impl::Car_store ts::resources::Car_store::car_store_;

void ts::resources::impl::Car_store::scan_directory(const std::string& directory)
{
    boost::filesystem::path dir_path(directory);
    for (boost::filesystem::directory_iterator it(dir_path), end; it != end; ++it) {
        if (it->path().extension() != ".car") continue;

        load_car_file(it->path().string());
    }
}

void ts::resources::impl::Car_store::load_car_file(const std::string& file_name)
{
    std::ifstream stream(file_name, std::ifstream::in);
    if (stream) {
        for (const auto& car_def : load_car_definitions(stream)) {
            auto car_name = car_def.car_name;
            boost::to_lower(car_name);

            car_map_[car_name] = car_def;
        }
    }
}

const ts::resources::Car_definition* ts::resources::impl::Car_store::get_car_by_name(std::string car_name) const
{
    boost::to_lower(car_name);

    auto it = car_map_.find(car_name);
    if (it != car_map_.end()) 
        return &it->second;

    return nullptr;
}

void ts::resources::Car_store::load(const std::string& directory)
{
    car_store_.scan_directory(directory);
}