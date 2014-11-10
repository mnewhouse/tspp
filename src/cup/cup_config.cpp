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
#include "cup_config.hpp"

#include "resources/resource_store.hpp"

ts::cup::Cup_config::Cup_config(resources::Resource_store* resource_store)
: car_settings_(&resource_store->car_settings()),
  track_settings_(&resource_store->track_settings()),
  script_settings_(&resource_store->script_settings())
{
}

ts::cup::Cup_config::~Cup_config()
{
}

const ts::resources::Car_settings& ts::cup::Cup_config::car_settings() const
{
    return *car_settings_;
}

const ts::resources::Track_settings& ts::cup::Cup_config::track_settings() const
{
    return *track_settings_;
}

const ts::resources::Script_settings& ts::cup::Cup_config::script_settings() const
{
    return *script_settings_;
}