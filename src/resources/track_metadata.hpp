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

#pragma once

#ifndef TRACK_METADATA_HPP
#define TRACK_METADATA_HPP

#include "track_identifier.hpp"

namespace ts
{
    namespace resources
    {
        class Track_handle;

        struct Track_metadata
        {
            Track_identifier identifier;
            utf8_string gamemode;
        };

        Track_metadata load_track_metadata(const resources::Track_handle& track_handle);
    }
}

#endif