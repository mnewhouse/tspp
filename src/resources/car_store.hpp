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

#ifndef RESOURCES_CAR_STORE_HPP
#define RESOURCES_CAR_STORE_HPP

#include <string>
#include <map>

namespace ts
{

    namespace resources
    {
        struct Car_definition;

        namespace impl
        {
            class Car_store
            {
            public:
                void scan_directory(const std::string& directory);

                const Car_definition* get_car_by_name(std::string car_name) const;

            private:
                void load_car_file(const std::string& file_name, const std::string& directory);

                std::map<std::string, Car_definition> car_map_;
            };

        }

        class Car_store
        {
        public:
            impl::Car_store* operator->() { return &car_store_; }

            void load(const std::string& car_directory);

        private:
            static impl::Car_store car_store_;
        };

    }

}

#endif
