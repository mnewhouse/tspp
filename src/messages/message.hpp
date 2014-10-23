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

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

namespace ts
{
    namespace messages
    {
        struct uint24_t
        {
            uint24_t(std::uint32_t value = 0);

            std::uint32_t value;
        };

        struct int24_t
        {
            int24_t(std::int32_t value = 0);

            std::int32_t value;
        };

        class Message
        {
        public:
            Message() = default;
            explicit Message(std::uint32_t type);
            Message(const std::uint8_t* data, std::size_t data_size);

            void assign(const std::uint8_t* data, std::size_t data_size);
            void append(const std::uint8_t* data, std::size_t data_size);
            void append(std::uint8_t byte);

            Message& operator<<(std::uint32_t value);
            Message& operator<<(std::int32_t value);

            Message& operator<<(std::uint64_t value);
            Message& operator<<(std::int64_t value);

            Message& operator<<(std::uint16_t value);
            Message& operator<<(std::int16_t value);

            Message& operator<<(std::uint8_t value);
            Message& operator<<(std::int8_t value);

            Message& operator<<(uint24_t value);
            Message& operator<<(int24_t value);

            Message& operator<<(const utf8_string& string);            

            const std::uint8_t* data() const;
            std::size_t size() const;

            void clear();
            bool empty() const;

            std::uint32_t type() const;

        private:
            std::vector<std::uint8_t> data_;
        };
    }
}

#endif