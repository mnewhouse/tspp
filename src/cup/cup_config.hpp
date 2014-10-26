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

#ifndef LOCAL_CUP_HPP
#define LOCAL_CUP_HPP

#include "resources/settings_copy.hpp"
#include "resources/settings/track_settings.hpp"
#include "resources/settings/car_settings.hpp"

namespace ts
{
    namespace resources
    {
        struct Resource_store;
    }

    namespace cup
    {
        class Cup_config
        {
        public:
            Cup_config(resources::Resource_store* resource_store);
            ~Cup_config();

            Cup_config(const Cup_config&) = delete;
            Cup_config& operator=(const Cup_config&) = delete;

            const resources::Car_settings& car_settings() const;
            const resources::Track_settings& track_settings() const;

        private:
            resources::Resource_store* resource_store_;

            template <typename T>
            using Settings_copy = resources::Settings_copy<T>;

            using Car_settings = resources::Car_settings;
            using Track_settings = resources::Track_settings;

            Settings_copy<Car_settings> car_settings_;
            Settings_copy<Track_settings> track_settings_;
        };
    }
}

#endif