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

#ifndef SCRIPT_DELEGATES_IMPL_HPP
#define SCRIPT_DELEGATES_IMPL_HPP

#include "delegate_definition.hpp"
#include "userdata_traits.hpp"
#include "delegate_traits.hpp"

namespace ts
{
    namespace script
    {
        namespace impl
        {
            template <typename T, typename OutIt>
            void copy_base_delegate_member_functions(OutIt out);

            template <typename T, typename OutIt>
            void copy_delegate_member_functions_impl(OutIt out);

            template <typename T, typename OutIt>
            void copy_delegate_member_functions(OutIt out)
            {
                copy_base_delegate_member_functions<T>(out);
                copy_delegate_member_functions_impl<T>(out);
            }

            template <typename T, typename OutIt>
            void copy_delegate_member_functions_impl(OutIt out)
            {
                auto member_functions = Delegate_traits<T>::member_functions();
                using std::begin;
                using std::end;

                std::copy(begin(member_functions), end(member_functions), out);
            }

            template <typename T, std::size_t Index, typename TupleType, typename OutIt>
            typename std::enable_if<Index < std::tuple_size<TupleType>::value>::type copy_base_delegate_member_functions_impl(OutIt out)
            {
                using elem_type = typename std::tuple_element<Index, TupleType>::type;

                copy_base_delegate_member_functions<elem_type>(out);
                copy_delegate_member_functions<elem_type>(out);

                copy_base_delegate_member_functions<T, Index + 1, TupleType>(out);
            }

            template <typename T, std::size_t Index, typename TupleType, typename OutIt>
            void copy_base_delegate_member_functions_impl(OutIt out, ...)
            {
            }           

            template <typename T, typename OutIt>
            void copy_base_delegate_member_functions(OutIt out)
            {
                using base_types = typename Userdata_traits<T>::base_types;

                copy_base_delegate_member_functions_impl<T, 0, base_types>(out);
            }
        }
    }
}

#endif
