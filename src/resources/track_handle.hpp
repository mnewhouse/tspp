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

#ifndef RESOURCES_TRACK_HANDLE_HPP
#define RESOURCES_TRACK_HANDLE_HPP

#include "track_identifier.hpp"

namespace ts
{
    namespace resources
    {        
        utf8_string path_to_track_name(const utf8_string& path);

        class Track_handle
        {
        public:
            explicit Track_handle(utf8_string path = utf8_string());

            utf8_string name() const;
            const utf8_string& path() const;

            operator bool() const;

        private:
            utf8_string track_path_;
            Track_identifier track_identifier_;
        };

        inline bool operator==(const Track_handle& a, const Track_handle& b)
        {
            return a.path() == b.path();
        }

        inline bool operator!=(const Track_handle& a, const Track_handle& b)
        {
            return !operator==(a, b);
        }

        inline bool operator<(const Track_handle& a, const Track_handle& b)
        {
            return a.path() < b.path();
        }

        inline bool operator>(const Track_handle& a, const Track_handle& b)
        {
            return a.path() > b.path();
        }

        inline bool operator>=(const Track_handle& a, const Track_handle& b)
        {
            return !operator<(a, b);
        }

        inline bool operator<=(const Track_handle& a, const Track_handle& b)
        {
            return !operator>(a, b);
        }        
    }
}

#endif