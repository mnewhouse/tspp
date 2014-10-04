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

namespace ts
{
    namespace resources
    {
        struct Resource_store;
    }

    namespace cup
    {
        class Cup;

        void load_cup_config(Cup* cup, const resources::Resource_store* resource_store);
        void save_cup_config(const Cup* cup, resources::Resource_store* resource_store);

        class Cup_config
        {
        public:
            Cup_config(Cup* cup, resources::Resource_store* resource_store);
            ~Cup_config();

            Cup_config(const Cup_config&) = delete;
            Cup_config& operator=(const Cup_config&) = delete;

        private:
            Cup* cup_;
            resources::Resource_store* resource_store_;
        };
    }
}

#endif