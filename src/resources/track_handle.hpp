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

#ifndef RESOURCES_TRACK_HANDLE_HPP
#define RESOURCES_TRACK_HANDLE_HPP

#include <string>

namespace ts
{
    namespace resources
    {        
        std::string path_to_track_name(const std::string& path);

        class Track_handle
        {
        public:
            explicit Track_handle(const std::string* path = nullptr);

            std::string name() const;
            const std::string& path() const;

            operator bool() const;

            bool operator==(Track_handle other);
        private:
            const std::string* track_path_;
        };

    }
}

#endif