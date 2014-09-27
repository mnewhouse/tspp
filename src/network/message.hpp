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

#ifndef NETWORK_MESSAGE_HPP
#define NETWORK_MESSAGE_HPP

namespace ts
{
    namespace network
    {
        enum class Message_protocol
        {
            Tcp,
            Udp
        };

        class Message
        {
        public:
            Message() = default;
            Message(const std::uint8_t* data, std::size_t data_size);

            void assign(const std::uint8_t* data, std::size_t data_size);

            const std::uint8_t* data() const;
            std::size_t size() const;

        private:
            std::vector<std::uint8_t> data_;
        };
    }
}

#endif