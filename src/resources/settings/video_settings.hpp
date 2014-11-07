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

#ifndef VIDEO_SETTINGS_HPP
#define VIDEO_SETTINGS_HPP

namespace ts
{
    namespace resources
    {
        class Video_settings
        {
        public:
            Video_settings();

            bool full_screen = true;
            bool vertical_sync = false;
            Vector2u screen_resolution;
            Vector2u current_screen_resolution;
            double zoom_level = 1.0;
            double split_screen_threshold = 0.7;
        };
    }
}

#endif

