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

#include <string>
#include <vector>
#include <map>

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
                std::string path;
                std::map<std::string, Track_directory> sub_directories;
                std::vector<std::string> track_files;
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

            private:
                friend Track_directory;
                track_iterator(std::vector<std::string>::const_iterator it);

                std::vector<std::string>::const_iterator it_;
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
                dir_iterator(std::map<std::string, impl::Track_directory>::const_iterator it);
                friend Track_directory;

                std::map<std::string, impl::Track_directory>::const_iterator it_;
            };

            Track_directory(const impl::Track_directory* track_dir);
            track_iterator track_begin() const;
            track_iterator track_end() const;

            dir_iterator dir_begin() const;
            dir_iterator dir_end() const;

            std::string dir_name() const;
            std::string path() const;

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
            void scan_directory(const std::string& directory);

            Track_handle get_track_by_name(std::string track_name) const;
            std::vector<Track_handle> get_matching_tracks(std::string partial_name) const;

            Track_handle random_track(const std::string& path) const;
            Track_handle random_track() const;

            Track_directory root_directory() const;

        private:
            void scan_directory(impl::Track_directory& dir_contents, const std::string& directory);

            impl::Track_directory root_directory_;
            std::map<std::string, std::string> lookup_map_;
        };

    }

}

#endif