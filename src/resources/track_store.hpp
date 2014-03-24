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

#ifndef RESOURCES_TRACK_STORE_HPP
#define RESOURCES_TRACK_STORE_HPP

#include <string>
#include <vector>
#include <map>

#include "track_handle.hpp"

namespace ts
{

    namespace resources
    {

        class Track_store
        {
        public:
            void scan_directory(const std::string& directory);
            ~Track_store() { printf("wtfff\n"); }

            Track_handle get_track_by_name(std::string track_name) const;
            std::vector<Track_handle> get_matching_tracks(std::string partial_name) const;

            Track_handle random_track(const std::string& path) const;
            Track_handle random_track() const;

        private:
            struct Track_directory
            {
                std::string path;
                std::map<std::string, Track_directory> sub_directories;
                std::vector<std::string> track_files;
            };

            void scan_directory(Track_directory& dir_contents, const std::string& directory);

            Track_directory root_directory_;
            std::map<std::string, std::string> lookup_map_;
        };

    /*
        class Track_store
        {
        public:
            impl::Track_store* operator->() { return &track_store_; }

            void load(const std::string& track_path);

        private:
            static impl::Track_store track_store_;
        };

        */
    }

}

#endif