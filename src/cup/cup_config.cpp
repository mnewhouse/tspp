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

#include "cup.hpp"

#include "resources/resource_store.hpp"
#include "resources/settings/track_settings.hpp"
#include "resources/settings/car_settings.hpp"

void ts::cup::load_cup_config(Cup* cup, const resources::Resource_store* resource_store)
{
    cup->load_track_settings(resource_store->track_settings());
    cup->load_car_settings(resource_store->car_settings());
}

void ts::cup::save_cup_config(const Cup* cup, resources::Resource_store* resource_store)
{
    resource_store->track_settings() = cup->track_settings();
    resource_store->car_settings() = cup->car_settings();
}

ts::cup::Cup_config::Cup_config(Cup* cup, resources::Resource_store* resource_store)
: cup_(cup), 
  resource_store_(resource_store)
{
    load_cup_config(cup, resource_store);
}

ts::cup::Cup_config::~Cup_config()
{
    save_cup_config(cup_, resource_store_);
}