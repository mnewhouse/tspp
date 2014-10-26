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

#ifndef RESOURCES_CAR_STORE_HPP
#define RESOURCES_CAR_STORE_HPP

#include "car_definition.hpp"
#include "car_handle.hpp"

namespace ts
{

    namespace resources
    {
        class Car_store
        {
        public:
            Car_store(const utf8_string& root_directory);
            Car_handle get_car_by_name(utf8_string car_name) const;

            struct iterator
                : public std::iterator<std::bidirectional_iterator_tag, Car_definition>
            {
            public:
                iterator(std::vector<Car_handle>::const_iterator it);

                Car_handle operator*() const;
                Car_handle operator->() const;
                
                iterator& operator++();
                iterator operator++(int);

                iterator& operator--();
                iterator operator--(int);

                bool operator==(const iterator& rhs) const;
                bool operator!=(const iterator& rhs) const;
                
            private:
                std::vector<Car_handle>::const_iterator it_;
            };

            iterator begin() const;
            iterator end() const;

            void scan_directory(const utf8_string& directory);
            const utf8_string& root_directory() const;

            void register_car_file(utf8_string relative_to_root);

        private:            
            void load_car_file(const boost::filesystem::path& file_name, const utf8_string& directory);

            std::map<utf8_string, Car_definition> car_map_;
            std::vector<Car_handle> car_list_;
            utf8_string root_directory_;
        };

    }

}

#endif
