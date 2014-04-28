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

#include "track_store.hpp"


#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <random>

namespace {
    const char track_extension[] = ".trk";

	std::mt19937 random_engine(std::random_device{}());
}

ts::resources::Track_handle ts::resources::Track_store::random_track() const
{
    if (lookup_map_.size() == 0) return Track_handle();

    std::uniform_int_distribution<std::size_t> distribution(0, lookup_map_.size() - 1);
    auto it = std::next(lookup_map_.begin(), distribution(random_engine));
    return Track_handle(&it->second);
}

void ts::resources::Track_store::scan_directory(const std::string& directory)
{
    root_directory_.sub_directories.clear();
    root_directory_.track_files.clear();

    try 
    {
        root_directory_.parent_directory = nullptr;
        scan_directory(root_directory_, directory);
    }

    catch (const boost::filesystem::filesystem_error&)
    {
        // Silently ignore errors, as if the directory were empty.
    }
}

void ts::resources::Track_store::scan_directory(impl::Track_directory& dir_contents, const std::string& directory)
{
    using boost::filesystem::directory_iterator;
    using boost::filesystem::directory_entry;

    dir_contents.path = directory;

    for (directory_iterator it(directory), end; it != end; ++it) {
        const auto& entry = *it;
        const auto& path = entry.path();
        const auto file_name = path.filename();        

        boost::filesystem::path full_path = directory;
        full_path /= file_name;

        if (is_directory(path)) {
            auto& sub_directory = dir_contents.sub_directories[file_name.string()];
            sub_directory.parent_directory = &dir_contents;
            scan_directory(sub_directory, full_path.string());
        }

        else if (is_regular_file(path) && file_name.extension() == track_extension) {
            dir_contents.track_files.push_back(path.string());

            auto track_name = basename(file_name);
            boost::to_lower(track_name);

            lookup_map_.insert(std::make_pair(track_name, dir_contents.track_files.back()));
        }
    }

    std::sort(dir_contents.track_files.begin(), dir_contents.track_files.end());
}

ts::resources::Track_handle ts::resources::Track_store::get_track_by_name(std::string track_name) const
{
    boost::to_lower(track_name);
    auto it = lookup_map_.find(track_name);
    if (it != lookup_map_.end()) {
        return Track_handle(&it->second);
    }

    return Track_handle();
}


std::vector<ts::resources::Track_handle> ts::resources::Track_store::get_matching_tracks(std::string partial_name) const
{
    std::vector<Track_handle> result;

    boost::to_lower(partial_name);
    std::transform(lookup_map_.lower_bound(partial_name), lookup_map_.upper_bound(partial_name), std::back_inserter(result), 
        [](const std::pair<std::string, std::string>& pair) { return Track_handle(&pair.second); });

    return result;
}

ts::resources::Track_directory ts::resources::Track_store::root_directory() const
{
    return Track_directory(&root_directory_);
}

ts::resources::Track_handle ts::resources::Track_directory::track_iterator::operator*() const
{
    return Track_handle(&*it_);
}

ts::resources::Track_directory::track_iterator& ts::resources::Track_directory::track_iterator::operator++()
{
    ++it_;
    return *this;
}

ts::resources::Track_directory::track_iterator ts::resources::Track_directory::track_iterator::operator++(int)
{
    auto temp = *this;
    ++*this;
    return temp;
}

ts::resources::Track_directory::track_iterator& ts::resources::Track_directory::track_iterator::operator--()
{
    --it_;
    return *this;
}

ts::resources::Track_directory::track_iterator ts::resources::Track_directory::track_iterator::operator--(int)
{
    auto temp = *this;
    --*this;
    return temp;
}

ts::resources::Track_directory::track_iterator& ts::resources::Track_directory::track_iterator::operator-=(std::ptrdiff_t diff)
{
    it_ -= diff;
    return *this;
}

ts::resources::Track_directory::track_iterator& ts::resources::Track_directory::track_iterator::operator+=(std::ptrdiff_t diff)
{
    it_ += diff;
    return *this;
}

bool ts::resources::Track_directory::track_iterator::operator==(track_iterator right) const
{
    return it_ == right.it_;
}

bool ts::resources::Track_directory::track_iterator::operator!=(track_iterator right) const
{
    return it_ != right.it_;
}

std::ptrdiff_t ts::resources::Track_directory::track_iterator::operator-(track_iterator right) const
{
    return it_ - right.it_;
}

ts::resources::Track_directory::track_iterator::track_iterator(std::vector<std::string>::const_iterator it)
: it_(it)
{
}

ts::resources::Track_directory::track_iterator ts::resources::Track_directory::track_begin() const
{
    return track_iterator(track_dir_->track_files.begin());
}

ts::resources::Track_directory::track_iterator ts::resources::Track_directory::track_end() const
{
    return track_iterator(track_dir_->track_files.end());
}

ts::resources::Track_directory::dir_iterator ts::resources::Track_directory::dir_begin() const
{
    return dir_iterator(track_dir_->sub_directories.begin());
}

ts::resources::Track_directory::dir_iterator ts::resources::Track_directory::dir_end() const
{
    return dir_iterator(track_dir_->sub_directories.end());
}

std::string ts::resources::Track_directory::path() const
{
    return track_dir_->path;
}

std::string ts::resources::Track_directory::dir_name() const
{
    return boost::filesystem::basename(track_dir_->path);
}


ts::resources::Track_directory ts::resources::Track_directory::sub_directory(const std::string& dir_name) const
{
    auto it = track_dir_->sub_directories.find(dir_name);
    if (it == track_dir_->sub_directories.end()) return Track_directory(nullptr);

    return Track_directory(&it->second);
}

ts::resources::Track_directory ts::resources::Track_directory::parent_directory() const
{
    if (track_dir_ == nullptr) return Track_directory(nullptr);

    return Track_directory(track_dir_->parent_directory);
}

ts::resources::Track_directory::dir_iterator::dir_iterator(std::map<std::string, impl::Track_directory>::const_iterator it)
    : it_(it)
{
}

ts::resources::Track_directory ts::resources::Track_directory::dir_iterator::operator*() const
{
    return Track_directory(&it_->second);
}

ts::resources::Track_directory::dir_iterator& ts::resources::Track_directory::dir_iterator::operator++()
{
    ++it_;
    return *this;
}

ts::resources::Track_directory::dir_iterator ts::resources::Track_directory::dir_iterator::operator++(int)
{
    auto temp = *this;
    ++*this;
    return temp;
}

ts::resources::Track_directory::dir_iterator& ts::resources::Track_directory::dir_iterator::operator--()
{
    --it_;
    return *this;
}

ts::resources::Track_directory::dir_iterator ts::resources::Track_directory::dir_iterator::operator--(int)
{
    auto temp = *this;
    --*this;
    return temp;
}

bool ts::resources::Track_directory::dir_iterator::operator==(dir_iterator right) const
{
    return it_ == right.it_;
}

bool ts::resources::Track_directory::dir_iterator::operator!=(dir_iterator right) const
{
    return it_ != right.it_;
}

ts::resources::Track_directory::Track_directory(const impl::Track_directory* track_dir)
    : track_dir_(track_dir)
{
}

ts::resources::Track_directory::operator bool() const
{
    return track_dir_ != nullptr;
}