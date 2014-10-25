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

#ifndef RESOURCES_TRACK_STORE_HPP
#define RESOURCES_TRACK_STORE_HPP

#include "track_handle.hpp"

namespace ts
{
    namespace resources
    {
        namespace impl
        {
            struct Track_directory
            {
                Track_directory* parent_directory;
                utf8_string path;
                std::map<utf8_string, Track_directory> sub_directories;
                std::vector<utf8_string> track_files;
            };
        }

        class Track_directory
        {
        public:            
            class track_iterator
                : public std::iterator<std::random_access_iterator_tag, Track_handle>
            {
            public:
                Track_handle operator*() const;
                track_iterator& operator++();
                track_iterator operator++(int);

                track_iterator& operator--();
                track_iterator operator--(int);

                track_iterator& operator-=(std::ptrdiff_t diff);
                track_iterator& operator+=(std::ptrdiff_t diff);

                std::ptrdiff_t operator-(track_iterator right) const;

                bool operator==(track_iterator right) const;
                bool operator!=(track_iterator right) const;

                bool operator<(track_iterator right) const;
                bool operator>(track_iterator right) const;
                bool operator<=(track_iterator right) const;
                bool operator>=(track_iterator right) const;

            private:
                friend Track_directory;
                track_iterator(std::vector<utf8_string>::const_iterator it);

                std::vector<utf8_string>::const_iterator it_;
            };
            
            class dir_iterator
                : public std::iterator<std::bidirectional_iterator_tag, Track_directory>
            {
            public:
                Track_directory operator*() const;

                dir_iterator& operator++();
                dir_iterator operator++(int);

                dir_iterator& operator--();
                dir_iterator operator--(int);

                bool operator==(dir_iterator right) const;
                bool operator!=(dir_iterator right) const;

            private:
                dir_iterator(std::map<utf8_string, impl::Track_directory>::const_iterator it);
                friend Track_directory;

                std::map<utf8_string, impl::Track_directory>::const_iterator it_;
            };

            Track_directory(const impl::Track_directory* track_dir);
            track_iterator track_begin() const;
            track_iterator track_end() const;

            dir_iterator dir_begin() const;
            dir_iterator dir_end() const;

            Range<dir_iterator> directories() const;
            Range<track_iterator> tracks() const;

            utf8_string dir_name() const;
            utf8_string path() const;

            Track_directory parent_directory() const;
            Track_directory sub_directory(const std::string& dir_name) const;
            operator bool() const;

        private:
            const impl::Track_directory* track_dir_;
        };

        Track_directory::track_iterator operator+(Track_directory::track_iterator it, std::ptrdiff_t offset);
        Track_directory::track_iterator operator+(std::ptrdiff_t offset, Track_directory::track_iterator it);
        Track_directory::track_iterator operator-(Track_directory::track_iterator it, std::ptrdiff_t offset);

        class Track_store
        {
        public:
            // Track_store() = default;
            Track_store(const utf8_string& root_directory);

            Track_handle get_track_by_name(utf8_string track_name) const;
            std::vector<Track_handle> get_matching_tracks(utf8_string partial_name) const;

            Track_handle random_track(const utf8_string& directory) const;
            Track_handle random_track() const;
            Track_handle random_track(const Track_directory& track_directory);

            Track_directory root_directory() const;
            Track_handle register_track(const utf8_string& path_relative_to_root);

            const utf8_string& root_directory_path() const;


            template <typename OutputIt>
            void select_random_tracks(const Track_directory& track_directory, std::size_t count, 
                                      OutputIt out, const std::set<Track_handle>& disallowed = {}) const;

            void scan_directory(const utf8_string& directory);

        private:
            void scan_directory(impl::Track_directory& dir_contents, const utf8_string& directory);

            impl::Track_directory root_directory_;
            utf8_string root_directory_path_;
            std::map<utf8_string, utf8_string> lookup_map_;

            mutable std::mt19937 random_engine_;
        };
    }
}


template <typename OutputIt>
void ts::resources::Track_store::select_random_tracks(const Track_directory& track_directory, std::size_t count, 
                                                      OutputIt out, const std::set<Track_handle>& disallowed) const
{
    std::vector<Track_handle> random_pool(track_directory.track_begin(), track_directory.track_end());

    auto is_disallowed = [&disallowed](const Track_handle& track_handle)
    {
        return disallowed.find(track_handle) != disallowed.end();
    };

    random_pool.erase(std::remove_if(random_pool.begin(), random_pool.end(), is_disallowed), random_pool.end());

    for (std::size_t n = 0; n != count && !random_pool.empty(); ++n)
    {
        std::uniform_int_distribution<std::int32_t> dist(0, static_cast<std::int32_t>(random_pool.size()) - 1);

        auto it = random_pool.begin() + dist(random_engine_);
        *out++ = *it;

        random_pool.erase(it);
    }
}

#endif