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
#include "resource_downloader.hpp"

#include "network/client.hpp"

ts::cup::Resource_downloader::Resource_downloader(network::Client* client)
: client_(client)
{
}

std::uint32_t ts::cup::Resource_downloader::generate_key() const
{
    std::uint32_t key = client_->generate_key() & 0xFFFFFFFF;
    while (download_map_.find(key) != download_map_.end())
    {
        key = client_->generate_key() & 0xFFFFFFFF;
    }

    return key;
}

void ts::cup::Resource_downloader::request_track(const resources::Track_definition& track_definition)
{
    auto key = generate_key();
    
}

bool ts::cup::Resource_downloader::is_downloading() const
{
    return !download_map_.empty();
}