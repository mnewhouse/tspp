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

#ifndef CORE_STREAM_UTILITY_HPP
#define CORE_STREAM_UTILITY_HPP

#include "utf8_string.hpp"

namespace ts
{
    namespace core
    {
        std::uint32_t read32(std::istream& stream);
        std::uint64_t read64(std::istream& stream);

        template <typename CharType>
        std::vector<CharType> read_stream_contents(std::basic_istream<CharType>& stream);

        template <typename CharType = unsigned char>
        std::vector<CharType> read_file_contents(const utf8_string& file_name);
    }   
}

template <typename CharType>
std::vector<CharType> ts::core::read_stream_contents(std::basic_istream<CharType>& stream)
{
    auto current_pos = stream.tellg();
    stream.seekg(0, std::istream::end);

    auto num_bytes = static_cast<std::size_t>(stream.tellg() - current_pos);

    stream.seekg(current_pos);
    std::vector<CharType> result(num_bytes);

    stream.read(result.data(), num_bytes);

    return result;
}

template <typename CharType>
std::vector<CharType> ts::core::read_file_contents(const utf8_string& file_name)
{
    boost::filesystem::basic_ifstream<CharType> stream(file_name.string(), std::istream::in | std::istream::binary);
    return read_stream_contents(stream);
}

#endif