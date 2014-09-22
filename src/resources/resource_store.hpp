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

#ifndef RESOURCES_RESOURCES_HPP
#define RESOURCES_RESOURCES_HPP

#include "track_store.hpp"
#include "car_store.hpp"
#include "player_store.hpp"
#include "script_manager.hpp"

#include "font_library.hpp"
#include "settings.hpp"

namespace ts
{
    namespace resources
    {
        struct Resource_store
        {
            Resource_store()
            : cars(config::car_directory),
              tracks(config::track_root_directory),
              players(config::players_file),
              scripts(config::script_root_directory),
              settings(config::settings_file, &cars, &tracks)
            {}

            Car_store cars;
            Track_store tracks;
            Player_store players;
            Script_manager scripts;
            Settings settings;
            Font_library font_library;
        };
    }
}

#endif