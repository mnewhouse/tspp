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
#include "settings.hpp"

#include "audio/audio_store.hpp"

namespace ts
{
    namespace resources
    {
        struct Resource_store
        {
            Track_store tracks;
            Car_store cars;
            Settings settings;

            audio::Audio_store sounds;
        };
    }
}

#endif