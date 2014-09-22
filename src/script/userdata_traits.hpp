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

#ifndef SCRIPT_USERDATA_TRAITS_HPP
#define SCRIPT_USERDATA_TRAITS_HPP

#include "squirrel_include.hpp"

namespace ts
{
    namespace script
    {
        namespace impl
        {
            template <typename T>
            struct Userdata_type_tag
            {
                static int dummy;
            };

            template <typename T>
            int Userdata_type_tag<T>::dummy;
        }

        template <typename T>
        SQUserPointer userdata_type_tag()
        {
            return reinterpret_cast<SQUserPointer>(&impl::Userdata_type_tag<T>::dummy);
        }


        template <typename T>
        struct Userdata_traits
        {
            using base_types = std::tuple<>;
            static const char* const type_name;
        };

        template <typename T>
        const char* const Userdata_traits<T>::type_name = "unknown userdata";
    }
}

#endif