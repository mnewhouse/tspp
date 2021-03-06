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

#ifndef STAGE_ASSEMBLER_API_HPP
#define STAGE_ASSEMBLER_API_HPP        

namespace ts
{
    namespace script
    {
        template <typename T>
        struct Userdata_forwarder
        {
            template <typename U>
            explicit Userdata_forwarder(U&& value, typename std::enable_if<!std::is_same<Userdata_forwarder<T>, typename std::decay<U>::type>::value>::type* = nullptr)
                : value(std::forward<U>(value))
            {}

            T value;
        };

        template <typename T>
        Userdata_forwarder<T&&> forward_as_userdata(T&& value)
        {
            return Userdata_forwarder<T&&>(std::forward<T>(value));
        }
    }
}

#endif