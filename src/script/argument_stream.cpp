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

#include "stdinc.hpp"
#include "argument_stream.hpp"

#include "script/script_utility.hpp"
#include "script/script_module.hpp"

ts::script::Argument_stream::Argument_stream(HSQUIRRELVM vm, utf8_string_view function_name)
: vm_(vm),
  current_argument_(1),
  argument_count_(sq_gettop(vm)),
  function_name_(function_name)
{
}

void ts::script::Argument_stream::error(SQInteger arg_index, const utf8_string& type_info)
{
    utf8_string error_string = "Bad argument [";
    error_string += std::to_string(arg_index - 1);
    error_string += "] to '";
    error_string += function_name_;
    error_string += "': expected '";
    error_string += type_info;
    error_string += "', got ";
    error_string += (arg_index <= argument_count_ ? get_value_string(vm_, arg_index) : "nothing");

    error_messages_.push_back(std::move(error_string));
}

ts::script::Argument_stream::operator bool() const
{
    return error_messages_.empty();
}

SQInteger ts::script::Argument_stream::argument_count() const
{
    return argument_count_;
}

const std::vector<ts::utf8_string>& ts::script::Argument_stream::error_messages() const
{
    return error_messages_;
}

namespace ts
{
    namespace script
    {
        const char* const Argument_reader_traits<Numeric_reader>::type_info = "number";
        const char* const Argument_reader_traits<String_reader>::type_info = "string";
        const char* const Argument_reader_traits<Tostring_reader>::type_info = "string";
        const char* const Argument_reader_traits<Value_reader>::type_info = "value";
        const char* const Argument_reader_traits<Function_reader>::type_info = "function";
    }
}

void ts::script::report_argument_errors(Module* module, const Argument_stream& argument_stream)
{
    for (const auto& error_message : argument_stream.error_messages())
    {
        module->report_error(error_message, Error_level::warning);
    }
}

bool ts::script::String_reader::operator()(HSQUIRRELVM vm, SQInteger index) const
{
    const SQChar* char_ptr{};

    if (SQ_SUCCEEDED(sq_getstring(vm, index, &char_ptr)))
    {
        auto length = sq_getsize(vm, index);
        result = utf8_string_view(char_ptr, char_ptr + length);
        return true;
    }

    return false;
}

bool ts::script::Tostring_reader::operator()(HSQUIRRELVM vm, SQInteger index) const
{
    Stack_guard stack_guard(vm);
    if (!SQ_SUCCEEDED(sq_tostring(vm, index)))
    {
        return false;
    }

    const SQChar* char_ptr{};
    sq_getstring(vm, -1, &char_ptr);
    auto length = sq_getsize(vm, -1);

    result = utf8_string_view(char_ptr, char_ptr + length);
    return true;
}

bool ts::script::Value_reader::operator()(HSQUIRRELVM vm, SQInteger index) const
{
    result = Value(vm, index);
    return true;
}

bool ts::script::Function_reader::operator()(HSQUIRRELVM vm, SQInteger index) const
{
    Function func(vm, index);
    if (func)
    {
        result = std::move(func);
        return true;
    }

    return false;
}