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
#include "script_manager.hpp"

ts::resources::Script_manager::Script_manager(const utf8_string& root_directory)
{
    load_resources(root_directory);
}

ts::resources::Script_handle ts::resources::Script_manager::get_script_by_name(const utf8_string& script_name) const
{
    auto it = loaded_scripts_.find(script_name);
    if (it == loaded_scripts_.end())
    {
        return Script_handle();
    }

    return Script_handle(&it->second);
}

const std::vector<ts::resources::Script_handle>& ts::resources::Script_manager::scripts() const
{
    return script_list_;
}

void ts::resources::Script_manager::load_resources(const utf8_string& root_directory)
{
    boost::filesystem::path directory_path = root_directory.string();

    for (boost::filesystem::directory_iterator dir_it(directory_path), dir_end; dir_it != dir_end; ++dir_it)
    {
        const auto& path = dir_it->path();

        if (boost::filesystem::is_directory(path))
        {
            utf8_string resource_name = boost::filesystem::basename(path);

            try
            {
                auto emplace_result = loaded_scripts_.emplace(std::piecewise_construct, std::forward_as_tuple(resource_name),
                                                              std::forward_as_tuple(path.string(), resource_name));

                if (emplace_result.second)
                {
                    script_list_.emplace_back(&emplace_result.first->second);
                }
            }

            catch (const Resource_config_exception& error)
            {
                std::cerr << error.what() << std::endl;
            }
        }
    }
}
