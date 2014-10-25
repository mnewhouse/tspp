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

#ifndef RESOURCES_TRACK_IDENTIFIER_HPP
#define RESOURCES_TRACK_IDENTIFIER_HPP

#include "track_hash.hpp"

namespace ts
{
    namespace resources
    {
        struct Track_identifier
        {
            utf8_string track_name;
            Track_hash track_hash;
        };

        inline bool operator==(const Track_identifier& lhs, const Track_identifier& rhs)
        {
            return lhs.track_name == rhs.track_name && lhs.track_hash == rhs.track_hash;
        }

        inline bool operator!=(const Track_identifier& lhs, const Track_identifier& rhs)
        {
            return !(lhs == rhs);
        }
    }
}

#endif