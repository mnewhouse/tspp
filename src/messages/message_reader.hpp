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

#ifndef MESSAGE_READER_HPP
#define MESSAGE_READER_HPP

namespace ts
{
    namespace messages
    {
        class Message;
        struct uint24_t;
        struct int24_t;

        class Message_reader
        {
        public:
            Message_reader(const Message& message);

            void advance(std::uint32_t offset);
            void reset(const Message& message);

            Message_reader& operator>>(std::uint64_t& value);
            Message_reader& operator>>(std::uint32_t& value);
            Message_reader& operator>>(std::uint16_t& value);
            Message_reader& operator>>(std::uint8_t& value);

            Message_reader& operator>>(std::int64_t& value);
            Message_reader& operator>>(std::int32_t& value);
            Message_reader& operator>>(std::int16_t& value);
            Message_reader& operator>>(std::int8_t& value);

            Message_reader& operator>>(uint24_t& value);
            Message_reader& operator>>(int24_t& value);

            Message_reader& operator>>(utf8_string& value);

            explicit operator bool() const;

        private:
            const std::uint8_t* ptr_;
            const std::uint8_t* end_;
            bool failbit_ = false;
        };
    }
}

#endif