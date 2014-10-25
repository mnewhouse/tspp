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
#include "include_path.hpp"

ts::utf8_string ts::resources::find_include_path(const utf8_string& file_name, std::initializer_list<utf8_string> search_paths)
{
    auto result = find_include_directory(file_name, search_paths);
    if (result.empty())
    {
        return result;
    }

    boost::filesystem::path path = result.string();
    path /= file_name.string();
    return path.string();
}

ts::utf8_string ts::resources::find_include_directory(const utf8_string& file_name, std::initializer_list<utf8_string> search_paths)
{
    for (const auto& path : search_paths)
    {
        boost::filesystem::path full_path = path.string();
        full_path /= file_name.string();

        if (is_regular_file(full_path)) return path;
    }

    return utf8_string();
}