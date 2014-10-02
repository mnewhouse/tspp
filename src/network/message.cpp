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
#include "message.hpp"

ts::network::Message::Message(std::uint32_t type)
{
    *this << type;
}

ts::network::Message::Message(const std::uint8_t* data, std::size_t data_size)
: data_(data, data + data_size)
{    
}

const std::uint8_t* ts::network::Message::data() const
{
    return data_.data();
}

std::size_t ts::network::Message::size() const
{
    return data_.size();
}

void ts::network::Message::clear()
{
    data_.clear();
}

std::uint32_t ts::network::Message::type() const
{
    if (data_.size() < 4)
    {
        return 0;
    }

    std::uint32_t result = 0;
    result |= static_cast<std::uint32_t>(data_[0]) << 24;
    result |= static_cast<std::uint32_t>(data_[1]) << 16;
    result |= static_cast<std::uint32_t>(data_[2]) << 8;
    result |= static_cast<std::uint32_t>(data_[3]);

    return result;
}

void ts::network::Message::assign(const std::uint8_t* data, std::size_t data_size)
{
    data_.assign(data, data + data_size);
}

void ts::network::Message::append(const std::uint8_t* data, std::size_t data_size)
{
    data_.insert(data_.end(), data, data + data_size);
}

void ts::network::Message::append(std::uint8_t byte)
{
    data_.push_back(byte);
}

ts::network::Message& ts::network::Message::operator<<(std::uint32_t value)
{
    const std::uint8_t bytes[] =
    {
        value >> 24 & 0xFF, value >> 16 & 0xFF, value >> 8 & 0xFF, value & 0xFF
    };

    append(bytes, sizeof(bytes));
    return *this;
}

ts::network::Message& ts::network::Message::operator<<(std::int32_t value)
{
    if (value >= 0)
    {
        return *this << static_cast<std::uint32_t>(value);
    }

    std::uint32_t uvalue = ~(-value - 1);
    return *this << uvalue;
}


ts::network::Message& ts::network::Message::operator<<(std::uint64_t value)
{
    const std::uint8_t bytes[] =
    {
        value >> 56 & 0xFF, value >> 48 & 0xFF, value >> 40 & 0xFF, value >> 32 & 0xFF,
        value >> 24 & 0xFF, value >> 16 & 0xFF, value >>  8 & 0xFF, value & 0xFF
    };

    append(bytes, sizeof(bytes));
    return *this;
}

ts::network::Message& ts::network::Message::operator<<(std::int64_t value)
{
    if (value >= 0)
    {
        return *this << static_cast<std::uint64_t>(value);
    }

    std::uint64_t uvalue = ~(-value - 1);
    return *this << uvalue;
}


ts::network::Message& ts::network::Message::operator<<(std::uint16_t value)
{
    const std::uint8_t bytes[] =
    {
        value >> 8 & 0xFF, value & 0xFF
    };

    append(bytes, sizeof(bytes));
    return *this;
}

ts::network::Message& ts::network::Message::operator<<(std::int16_t value)
{
    if (value >= 0)
    {
        return *this << static_cast<std::uint16_t>(value);
    }

    std::uint16_t uvalue = ~(-value - 1);
    return *this << uvalue;
}


ts::network::Message& ts::network::Message::operator<<(std::uint8_t value)
{
    append(value);
    return *this;
}

ts::network::Message& ts::network::Message::operator<<(std::int8_t value)
{
    if (value >= 0)
    {
        return *this << static_cast<std::uint8_t>(value);
    }

    std::uint8_t uvalue = ~(-value - 1);
    return *this << uvalue;
}

ts::network::Message& ts::network::Message::operator<<(const utf8_string& u8_string)
{
    const auto& string = u8_string.string();
    *this << static_cast<std::int32_t>(string.size());
    
    append(reinterpret_cast<const std::uint8_t*>(string.data()), string.size());
    return *this;
}