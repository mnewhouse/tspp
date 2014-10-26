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
#include "car_store.hpp"
#include "car_definition.hpp"
#include "pattern.hpp"

ts::resources::Car_store::Car_store(const utf8_string& root_directory)
: root_directory_(root_directory)
{
    scan_directory(root_directory);
}

void ts::resources::Car_store::scan_directory(const utf8_string& directory)
{
    boost::filesystem::path dir_path(directory.string());
    try 
    {
        for (boost::filesystem::directory_iterator it(dir_path), end; it != end; ++it) 
        {
            if (it->path().extension() != ".car") continue;

            load_car_file(it->path().string(), directory);
        }
    }
    
    catch (const boost::filesystem::filesystem_error&)
    {
        // Silently ignore errors, as if the directory were empty.
    }
}

void ts::resources::Car_store::load_car_file(const boost::filesystem::path& file_path, const utf8_string& directory)
{
    boost::filesystem::ifstream stream(file_path, std::ifstream::in);
    if (stream) 
    {
        for (auto& car_def : load_car_definitions(stream, directory)) 
        {
            auto car_name = to_lower(car_def.car_name);
            
            auto& entry = car_map_[std::move(car_name)];
            entry = car_def;
            car_list_.emplace_back(&entry);            
        }
    }
}

void ts::resources::Car_store::register_car_file(utf8_string relative_to_root)
{
    auto path = boost::filesystem::path(root_directory_.string()) / relative_to_root.string();

    load_car_file(path, path.parent_path().string());
}

const ts::utf8_string& ts::resources::Car_store::root_directory() const
{
    return root_directory_;
}

ts::resources::Car_handle ts::resources::Car_store::get_car_by_name(utf8_string car_name) const
{
    car_name = to_lower(car_name);

    auto it = car_map_.find(car_name);
    if (it == car_map_.end())
    {
        return Car_handle();
    }

    return Car_handle(&it->second);
}

ts::resources::Car_store::iterator ts::resources::Car_store::begin() const
{
    return iterator(car_list_.begin());
}

ts::resources::Car_store::iterator ts::resources::Car_store::end() const
{
    return iterator(car_list_.end());
}

ts::resources::Car_store::iterator::iterator(std::vector<Car_handle>::const_iterator it)
: it_(it)
{
}

ts::resources::Car_store::iterator& ts::resources::Car_store::iterator::operator++()
{
    ++it_;
    return *this;
}

ts::resources::Car_store::iterator& ts::resources::Car_store::iterator::operator--()
{
    --it_;
    return *this;
}

ts::resources::Car_store::iterator ts::resources::Car_store::iterator::operator++(int)
{
    auto temp = *this;
    ++*this;
    return temp;
}

ts::resources::Car_store::iterator ts::resources::Car_store::iterator::operator--(int)
{
    auto temp = *this;
    --*this;
    return temp;
}

ts::resources::Car_handle ts::resources::Car_store::iterator::operator*() const
{
    return *it_;
}

ts::resources::Car_handle ts::resources::Car_store::iterator::operator->() const
{
    return *it_;
}

bool ts::resources::Car_store::iterator::operator==(const iterator& rhs) const
{
    return it_ == rhs.it_;
}

bool ts::resources::Car_store::iterator::operator!=(const iterator& rhs) const
{
    return it_ != rhs.it_;
}