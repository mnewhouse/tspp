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
#include "stream_utility.hpp"


std::uint32_t ts::core::read32(std::istream& stream)
{
    char buffer[4];
    stream.read(buffer, 4);

    std::uint32_t result = 0;
    if (stream.gcount() == 4)
    {
        std::uint32_t shift_amount = 24;
        for (std::uint8_t ch : buffer)
        {
            result |= std::uint32_t(ch) << shift_amount;
            shift_amount -= 8;
        }
    }

    return result;
};

std::uint64_t ts::core::read64(std::istream& stream)
{
    char buffer[8];
    stream.read(buffer, 8);

    std::uint64_t result = 0;
    if (stream.gcount() == 8)
    {
        std::uint32_t shift_amount = 56;
        for (std::uint8_t ch : buffer)
        {
            result |= std::uint64_t(ch) << shift_amount;
            shift_amount -= 8;
        }
    }

    return result;
};

std::vector<char> ts::core::read_stream_contents(std::istream& stream)
{
    auto current_pos = stream.tellg();
    stream.seekg(0, std::istream::end);

    auto num_bytes = static_cast<std::size_t>(stream.tellg() - current_pos);

    stream.seekg(current_pos);
    std::vector<char> result(num_bytes);

    stream.read(result.data(), num_bytes);

    return result;

}

std::vector<char> ts::core::read_file_contents(const utf8_string& file)
{
    boost::filesystem::ifstream stream(file.string(), std::istream::in | std::istream::binary);
    return read_stream_contents(stream);
}