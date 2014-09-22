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

#ifndef SCRIPT_INTERFACE_HPP
#define SCRIPT_INTERFACE_HPP

#include "squirrel_include.hpp"

namespace ts
{
    namespace script
    {
        namespace impl
        {
            template <typename T>
            struct Interface_tag
            {
                static int dummy;
            };

            template <typename T>
            int Interface_tag<T>::dummy;

            template <typename T>
            SQUserPointer interface_tag()
            {
                return reinterpret_cast<SQUserPointer>(&Interface_tag<T>::dummy);
            };
        }

        template <typename T>
        using Interface = Pointer_handle<T>;

        class Generic_interface
            : public Pointer_handle<void>
        {
        public:
            Generic_interface(SQUserPointer pointer, SQUserPointer tag)
                : Pointer_handle(pointer),
                  interface_tag_(tag)
            {}

            Generic_interface() = default;

            template <typename T>
            Generic_interface(Interface<T> interface)
                : Pointer_handle(interface.get()),
                  interface_tag_(impl::interface_tag<T>())
            {}

            SQUserPointer tag() const
            {
                return interface_tag_;
            }

        private:
            SQUserPointer interface_tag_ = nullptr;
        };

        template <typename T>
        Interface<T> interface_cast(const Generic_interface& interface)
        {
            if (interface.tag() != impl::interface_tag<T>()) return Interface<T>();

            auto ptr = reinterpret_cast<T*>(interface.get());
            return Interface<T>(ptr);
        }

        void register_interface(HSQUIRRELVM vm, const Generic_interface& interface);
        Generic_interface get_interface(HSQUIRRELVM vm, SQUserPointer interface_type);

        template <typename T>
        Interface<T> get_interface(HSQUIRRELVM vm)
        {
            return interface_cast<T>(get_interface(vm, impl::interface_tag<T>()));
        }

        template <typename T>
        Interface<T> make_interface(T* ptr)
        {
            return Interface<T>(ptr);
        }
    }
}

        
#endif