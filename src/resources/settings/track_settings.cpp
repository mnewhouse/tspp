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
#include "track_settings.hpp"


void ts::resources::Track_settings::add_track(Track_handle track)
{
    selected_tracks_.push_back(track);
}

void ts::resources::Track_settings::add_track_if_not_selected(Track_handle track)
{
    if (!is_selected(track))
    {
        add_track(track);
    }
}

bool ts::resources::Track_settings::is_selected(Track_handle track) const
{
    return std::find(selected_tracks_.begin(), selected_tracks_.end(), track) != selected_tracks_.end();
}

void ts::resources::Track_settings::remove_track(Track_handle track)
{
    selected_tracks_.erase(std::remove(selected_tracks_.begin(), selected_tracks_.end(), track), selected_tracks_.end());
}

void ts::resources::Track_settings::remove_track(std::size_t index)
{
    if (index < selected_tracks_.size())
    {
        selected_tracks_.erase(selected_tracks_.begin() + index);
    }
}

void ts::resources::Track_settings::move_forward(std::size_t index)
{
    if (index < selected_tracks_.size() - 1)
    {
        std::swap(selected_tracks_[index], selected_tracks_[index + 1]);
    }
}

void ts::resources::Track_settings::move_backward(std::size_t index)
{
    if (index > 0)
    {
        std::swap(selected_tracks_[index - 1], selected_tracks_[index]);
    }
}

void ts::resources::Track_settings::clear_selection()
{
    selected_tracks_.clear();
}

const std::vector<ts::resources::Track_handle>& ts::resources::Track_settings::selected_tracks() const
{
    return selected_tracks_;
}