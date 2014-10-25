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

#ifndef RESOURCES_CAR_IDENTIFIER_HPP
#define RESOURCES_CAR_IDENTIFIER_HPP

namespace ts
{
    namespace resources
    {
        struct Car_identifier
        {
            utf8_string car_name;
        };

        inline bool operator==(const Car_identifier& lhs, const Car_identifier& rhs)
        {
            return lhs.car_name == rhs.car_name;
        }

        inline bool operator!=(const Car_identifier& lhs, const Car_identifier& rhs)
        {
            return !(lhs == rhs);
        }
    }
}

#endif