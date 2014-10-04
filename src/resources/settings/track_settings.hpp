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

#ifndef TRACK_SETTINGS_HPP
#define TRACK_SETTINGS_HPP

#include "resources/track_handle.hpp"

namespace ts
{
    namespace resources
    {
        class Track_settings
        {
        public:
            void add_track(Track_handle track);
            void add_track_if_not_selected(Track_handle track);

            bool is_selected(Track_handle track) const;
            void remove_track(Track_handle track);
            void remove_track(std::size_t index);

            void move_forward(std::size_t index);
            void move_backward(std::size_t index);

            void clear_selection();

            const std::vector<Track_handle>& selected_tracks() const;

        private:
            std::vector<Track_handle> selected_tracks_;
        };
    }
}


#endif