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

#ifndef SCRIPT_VALUE_HPP
#define SCRIPT_VALUE_HPP

#include <boost/variant.hpp>

#include "squirrel_include.hpp"
#include "script_function.hpp"
#include "script_userdata.hpp"

namespace ts
{
    namespace script
    {        
        class Value
        {
        public:
            enum Type
            {
                null_type,
                bool_type,
                integer_type,
                float_type,
                string_type,
                string_view_type,
                userdata_type,
                userpointer_type,
                object_type,
                function_type
            };

            using null_t = std::nullptr_t;
            using bool_t = bool;
            using integer_t = SQInteger;
            using float_t = SQFloat;
            using string_t = utf8_string;
            using string_view_t = utf8_string_view;
            using function_t = Function;
            using object_t = Object_handle;
            using userdata_t = Generic_userdata;
            using userpointer_t = SQUserPointer;

        private:
            template <typename T>
            struct type_conversion
            {
                template <typename U, typename = std::enable_if_t<std::is_integral<std::decay_t<U>>::value &&
                    !std::is_same<std::decay_t<U>, bool_t>::value>>
                static integer_t f(int);

                template <typename U>
                static T f(...);

                using type = decltype(f<T>(0));
            };

            template <typename T>
            using type_conversion_t = typename type_conversion<T>::type;


        public:
            Value();
            Value(HSQUIRRELVM vm, SQInteger index);

            Value(const Value& value) = default;
            Value& operator=(const Value& value) = default;

            template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, Value>::value>>
            Value(T&& value) 
                : variant_(static_cast<type_conversion_t<T&&>>(value)) 
            {
            }

            template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, Value>::value>>
            Value& operator=(T&& value)
            {
                variant_ = static_cast<type_conversion_t<T&&>>(value);
                return *this;
            }


            Type type() const;
            void push(HSQUIRRELVM vm) const;

        private:
            using variant_t = boost::variant<null_t, bool_t, integer_t, float_t, string_t, string_view_t, userdata_t, userpointer_t, object_t, function_t>;

            struct type_visitor;
            struct push_visitor;
            struct copy_visitor;
            friend Value copy_value(const Value&, HSQUIRRELVM);

            variant_t read_value_from_stack(HSQUIRRELVM vm, SQInteger index);
            
            variant_t variant_;
        };

        Value copy_value(const Value& value, HSQUIRRELVM target_vm);
    }
}

#endif