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

#ifndef CORE_CONFIG_HPP
#define CORE_CONFIG_HPP

namespace ts
{

    namespace config
    {

        // Various constants that are known at compile-time.

        static const char* const window_title = "Turbo Sliders++";

        static const char* const track_root_directory = "tracks";
        static const char* const data_directory = "data";
        static const char* const car_directory = "cars";
        static const char* const sound_directory = "sound";

        static const char* const default_collision_sound = "collision.wav";
        static const char* const default_entity_collision_sound = "carcollision.wav";
        static const char* const default_skid_sound = "skid.wav";

        static const char* const settings_file = "settings.ini";

    }

}

#endif