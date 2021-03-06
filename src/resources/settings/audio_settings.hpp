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

#ifndef AUDIO_SETTINGS_HPP
#define AUDIO_SETTINGS_HPP

namespace ts
{
    namespace resources
    {
        class Audio_settings
        {
        public:
            double effects_volume = 0.8;
            double engine_volume = 0.8;
            double remote_engine_volume = 0.5;

            std::size_t engine_channels = 8;
            std::size_t effects_channels = 8;
        };
    }
}

#endif