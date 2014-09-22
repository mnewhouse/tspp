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

#ifndef SCRIPT_USERDATA_IMPL_HPP
#define SCRIPT_USERDATA_IMPL_HPP

#include "squirrel_include.hpp"
#include "userdata_traits.hpp"

namespace ts
{
    namespace script
    {
        namespace impl
        {            
            template <typename T, typename Base>
            T* base_cast_impl(Base* base_ptr, typename std::enable_if<std::is_polymorphic<Base>::value, int>::type)
            {
                return dynamic_cast<T*>(base_ptr);
            }

            template <typename T, typename Base>
            T* base_cast_impl(Base* base_ptr, typename std::enable_if<std::is_base_of<Base, T>::value, long>::type)
            {
                return static_cast<T*>(base_ptr);
            }

            template <typename T, typename Base>
            T* base_cast_impl(Base* base_ptr, ...)
            {
                return nullptr;
            }

            template <typename T, std::size_t Index, typename TupleType>
            typename std::enable_if<Index < std::tuple_size<TupleType>::value, T*>::type 
                dynamic_userdata_cast(SQUserPointer userdata, SQUserPointer type_tag)
            {
                using base_type = typename std::tuple_element<TupleType>::type;
                using traits_type = Userdata_traits<base_type>;

                if (type_tag == userdata_type_tag<base_type>())
                {
                    base_type* base_ptr = static_cast<base_type*>(userdata);
                    if (T* result = base_cast_impl<T>(base_ptr, 0))
                    {
                        return result;
                    }
                }

                using base_types = traits_type::base_types;
                if (T* result = dynamic_userdata_cast<T, std::tuple_size<base_types>::value - 1, base_types>(userdata, type_tag))
                {
                    return result;
                }

                return dynamic_userdata_cast<T, Index - 1, TupleType>(userdata, type_tag);
            }

            template <typename T, std::size_t Index, typename TupleType>
            T* dynamic_userdata_cast(SQUserPointer userdata, SQUserPointer type_tag, ...)
            {
                return nullptr;
            }

           template <typename T>
           T* userdata_cast(SQUserPointer userdata, SQUserPointer type_tag)
           {
               if (userdata_type_tag<T>() == type_tag)
               {
                   return static_cast<T*>(userdata);
               }

               using base_types = typename Userdata_traits<T>::base_types;
               return impl::dynamic_userdata_cast<T, std::tuple_size<base_types>::value - 1, base_types>(userdata, type_tag);
           }
        }
    }
}

#endif
