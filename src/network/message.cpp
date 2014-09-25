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

void ts::network::Message::assign(const std::uint8_t* data, std::size_t data_size)
{
    data_.assign(data, data + data_size);
}