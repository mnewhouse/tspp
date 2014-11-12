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
#include "track_metadata.hpp"
#include "track_handle.hpp"

namespace ts
{
    namespace resources
    {
        static Track_metadata load_track_metadata(std::istream& stream, const utf8_string& track_name);
    }
}

ts::resources::Track_metadata ts::resources::load_track_metadata(const Track_handle& track_handle)
{
    boost::filesystem::ifstream stream(track_handle.path().string());
    return load_track_metadata(stream, track_handle.name());
}

ts::resources::Track_metadata ts::resources::load_track_metadata(std::istream& stream, const utf8_string& track_name)
{
    Track_metadata metadata;
    metadata.identifier.track_name = track_name;

    for (std::string line, directive, param; std::getline(stream, line);)
    {
        std::istringstream line_stream(line);
        read_directive(line_stream, directive);

        if (directive == "gamemode")
        {
            line_stream >> std::ws;
            if (std::getline(line_stream, param))
            {
                metadata.gamemode = std::move(param);
            }
        }

        else if (directive == "trackhash")
        {
            auto& track_hash = metadata.identifier.track_hash;
            for (std::uint32_t index = 0, value = 0; index != track_hash.size() && line_stream >> std::hex >> value; ++index)
            {
                track_hash[index] = value;
            }
        }
    }

    return metadata;
}