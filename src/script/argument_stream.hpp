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

#ifndef SCRIPT_ARGUMENT_STREAM_HPP
#define SCRIPT_ARGUMENT_STREAM_HPP

#include "squirrel_include.hpp"

#include "script_function.hpp"
#include "script_value.hpp"

#include <boost/optional.hpp>

namespace ts
{
    namespace script
    {
        template <typename ReaderType>
        struct Argument_reader_traits
        {
            static const utf8_string type_info;
        };

        template <typename ReaderType>
        const utf8_string Argument_reader_traits<ReaderType>::type_info = "unknown";

        namespace arg
        {
            struct optional_t {};
            static const optional_t optional;
        }

        class Argument_stream
        {
        public:
            Argument_stream(HSQUIRRELVM vm, utf8_string_view function_name = utf8_string_view());

            template <typename ReaderType>
            bool operator()(ReaderType&& reader, arg::optional_t optional)
            {
                auto arg_index = current_argument_++;
                if (arg_index <= argument_count_ && reader(vm_, arg_index))
                {
                    return true;
                }

                return false;
            }

            template <typename ReaderType>
            bool operator()(ReaderType&& reader)
            {
                using traits_type = Argument_reader_traits<std::remove_reference_t<ReaderType>>;

                auto arg_index = current_argument_++;
                if (arg_index <= argument_count_ && reader(vm_, arg_index))
                {
                    return true;
                }

                error(arg_index, traits_type::type_info);
                return false;
            }

            explicit operator bool() const;
            const std::vector<utf8_string>& error_messages() const;

            SQInteger argument_count() const;

        private:
            void error(SQInteger arg_index, const utf8_string& type_info);

            HSQUIRRELVM vm_;
            SQInteger current_argument_;
            SQInteger argument_count_;
            utf8_string_view function_name_;

            std::vector<utf8_string> error_messages_;
        };

        void report_argument_errors(Module* module, const Argument_stream& argument_stream);

        struct Ignore_argument_t
        {
            bool operator()(HSQUIRRELVM vm, SQInteger index) const
            {
                return true;
            }
        };

        static const Ignore_argument_t ignore_argument;

        template <typename IntegerType>
        struct Integer_reader
        {
            Integer_reader(IntegerType& result)
                : result_(result)
            {}

            bool operator()(HSQUIRRELVM vm, SQInteger index) const
            {
                SQInteger integer;
                if (SQ_SUCCEEDED(sq_getinteger(vm, index, &integer)))
                {
                    result_ = static_cast<IntegerType>(integer);
                    return true;
                }

                return false;
            }

        private:
            IntegerType& result_;
        };

        template <typename IntegerType>
        Integer_reader<IntegerType> make_integer_reader(IntegerType& out)
        {
            return Integer_reader<IntegerType>(out);
        }

        struct Numeric_reader
        {
            Numeric_reader(double& result)
                : result_(result)
            {}

            bool operator()(HSQUIRRELVM vm, SQInteger index) const;

        private:
            double& result_;
        };

        struct Tostring_reader
        {
            Tostring_reader(utf8_string_view& result)
                : result_(result)
            {}

            bool operator()(HSQUIRRELVM vm, SQInteger index) const;

        private:
            utf8_string_view& result_;
        };

        struct String_reader
        {
            String_reader(utf8_string_view& result)
                : result_(result)
            {}            

            bool operator()(HSQUIRRELVM vm, SQInteger index) const;

        private:
            utf8_string_view& result_;
        };

        struct Value_reader
        {
            Value_reader(Value& result)
              : result_(result)
            {                
            }

            bool operator()(HSQUIRRELVM vm, SQInteger index) const;

        private:
            Value& result_;
        };

        struct Function_reader
        {
            Function_reader(Function& result)
                : result_(result)
            {}

            bool operator()(HSQUIRRELVM vm, SQInteger index) const;

        private:
            Function& result_;
        };

        template <> 
        struct Argument_reader_traits<Numeric_reader>
        {
            static const char* const type_info;
        };

        template <> 
        struct Argument_reader_traits<String_reader>
        {
            static const char* const type_info;
        };

        template <>
        struct Argument_reader_traits<Tostring_reader>
        {
            static const char* const type_info;
        };

        template <> 
        struct Argument_reader_traits<Value_reader>
        {
            static const char* const type_info;
        };

        template <> 
        struct Argument_reader_traits<Function_reader>
        {
            static const char* const type_info;
        };
    }
}

#endif