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
#include "track_handle.hpp"

ts::utf8_string ts::resources::path_to_track_name(const utf8_string& path)
{
    return boost::filesystem::basename(path.string());
}

ts::resources::Track_handle::Track_handle(utf8_string path)
: track_path_(std::move(path))
{
}

ts::utf8_string ts::resources::Track_handle::name() const
{
    return path_to_track_name(path());
}

const ts::utf8_string& ts::resources::Track_handle::path() const
{
    return track_path_;
}

ts::resources::Track_handle::operator bool() const
{
    return !track_path_.empty();
}