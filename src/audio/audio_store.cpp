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


#include "audio_store.hpp"

#include <boost/filesystem.hpp>
#include <iostream>

ts::audio::Audio_store::Audio_store(std::string search_directory)
: search_directory_(std::move(search_directory))
{
}

ts::audio::Audio_handle ts::audio::Audio_store::operator[](std::string file_name)
{
    if (!search_directory_.empty())
    {
        const auto& path = boost::filesystem::path(search_directory_) / file_name;
        file_name = path.string();
    }

    auto it = loaded_samples_.find(file_name);
    if (it != loaded_samples_.end()) return it->second;

    auto audio_handle = std::make_shared<sf::SoundBuffer>();

    if (!audio_handle->loadFromFile(file_name))
    {
        throw Load_error(file_name);
    }

    auto result = loaded_samples_.insert(std::make_pair(file_name, audio_handle));
    return audio_handle;
}