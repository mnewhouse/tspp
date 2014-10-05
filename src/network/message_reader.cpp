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
#include "message_reader.hpp"
#include "message.hpp"

ts::network::Message_reader::Message_reader(const Message& message)
: ptr_(message.data()),
  end_(message.data() + message.size())
{
}

ts::network::Message_reader::operator bool() const
{
    return !failbit_;
}

void ts::network::Message_reader::reset(const Message& message)
{
    failbit_ = false;
    ptr_ = message.data();
    end_ = ptr_ + message.size();
}

void ts::network::Message_reader::advance(std::uint32_t offset)
{
    ptr_ += offset;
}

ts::network::Message_reader& ts::network::Message_reader::operator>>(std::uint64_t& value)
{
    if (!*this || end_ - ptr_ < 8)
    {
        failbit_ = true;
        return *this;
    }

    value = 0;
    value |= static_cast<std::uint64_t>(*ptr_++) << 56;
    value |= static_cast<std::uint64_t>(*ptr_++) << 48;
    value |= static_cast<std::uint64_t>(*ptr_++) << 40;
    value |= static_cast<std::uint64_t>(*ptr_++) << 32;
    value |= static_cast<std::uint64_t>(*ptr_++) << 24;
    value |= static_cast<std::uint64_t>(*ptr_++) << 16;
    value |= static_cast<std::uint64_t>(*ptr_++) <<  8;
    value |= static_cast<std::uint64_t>(*ptr_++);

    return *this;
}


ts::network::Message_reader& ts::network::Message_reader::operator>>(std::uint32_t& value)
{
    if (!*this || end_ - ptr_ < 4)
    {
        failbit_ = true;
        return *this;
    }
    
    value = 0;
    value |= static_cast<std::uint32_t>(*ptr_++) << 24;
    value |= static_cast<std::uint32_t>(*ptr_++) << 16;
    value |= static_cast<std::uint32_t>(*ptr_++) <<  8;
    value |= static_cast<std::uint32_t>(*ptr_++);

    return *this;
}

ts::network::Message_reader& ts::network::Message_reader::operator>>(std::uint16_t& value)
{
    if (!*this || end_ - ptr_ < 2)
    {
        failbit_ = true;
        return *this;
    }
    
    value = 0;
    value |= static_cast<std::uint16_t>(*ptr_++) <<  8;
    value |= static_cast<std::uint16_t>(*ptr_++);

    return *this;
}

ts::network::Message_reader& ts::network::Message_reader::operator>>(std::uint8_t& value)
{
    if (!*this || end_ - ptr_ < 1)
    {
        failbit_ = true;
        return *this;
    }
    
    value = 0;
    value |= *ptr_++;

    return *this;
}


ts::network::Message_reader& ts::network::Message_reader::operator>>(std::int64_t& value)
{
    std::uint64_t uvalue;
    if (*this >> uvalue)
    {
        if (uvalue >> 63)
        {
            // Sign bit is set
            value = -static_cast<std::int64_t>(~uvalue + 1);
        }

        else
        {
            value = uvalue;
        }
    }

    return *this;
}

ts::network::Message_reader& ts::network::Message_reader::operator>>(std::int32_t& value)
{
    std::uint32_t uvalue;
    if (*this >> uvalue)
    {
        if (uvalue >> 31)
        {
            // Sign bit is set
            value = -static_cast<std::int32_t>(~uvalue + 1);
        }

        else
        {
            value = uvalue;
        }
    }

    return *this;
}

ts::network::Message_reader& ts::network::Message_reader::operator>>(std::int16_t& value)
{
    std::uint16_t uvalue;
    if (*this >> uvalue)
    {
        if (uvalue >> 15)
        {
            // Sign bit is set
            value = -static_cast<std::int16_t>(~uvalue + 1);
        }

        else
        {
            value = uvalue;
        }
    }

    return *this;
}


ts::network::Message_reader& ts::network::Message_reader::operator>>(std::int8_t& value)
{
    std::uint8_t uvalue;
    if (*this >> uvalue)
    {
        if (uvalue >> 7)
        {
            // Sign bit is set
            value = -static_cast<std::int8_t>(~uvalue + 1);
        }

        else
        {
            value = uvalue;
        }
    }

    return *this;
}

ts::network::Message_reader& ts::network::Message_reader::operator>>(utf8_string& value)
{
    std::int32_t length;
    if (*this >> length)
    {
        length = clamp(length, 0, 1024);

        if (end_ - ptr_ < length)
        {
            failbit_ = true;
        }

        else
        {
            value = std::string(ptr_, ptr_ + length);
            ptr_ += length;
        }
    }

    return *this;
}