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

#ifndef TRACK_LOADER_HPP
#define TRACK_LOADER_HPP

namespace ts
{
    namespace resources
    {
        class Track;
        class Track_handle;

        struct Broken_track_exception
            : public std::logic_error
        {
            Broken_track_exception();
        };

        class Track_loader
        {
        public:
            Track_loader();
            ~Track_loader();

            void load_from_handle(const resources::Track_handle& track_handle);
            void load_from_file(const utf8_string& file_name);
            void load_from_stream(std::istream& stream, utf8_string working_directory);

            Track get_result();
            const std::vector<utf8_string>& assets() const;

        private:
            void include(const utf8_string& file_name);

            struct Impl;
            std::unique_ptr<Impl> impl_;
        };
    }
}

#endif