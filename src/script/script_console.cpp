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
#include "script_console.hpp"

ts::script::Console::Console(const Console& other)
: self_(other.self_->clone_())
{
}

ts::script::Console::Console(Console&& other)
: self_(std::move(other.self_))
{
}

ts::script::Console& ts::script::Console::operator=(const Console& other)
{
    self_ = other.self_->clone_();

    return *this;
}

ts::script::Console& ts::script::Console::operator=(Console&& other)
{
    self_ = std::move(other.self_);

    return *this;
}

void ts::script::Console::write_line(const utf8_string& string, Error_level error_level)
{
    self_->write_line_(string, error_level);
}

void ts::script::write_line(const Stdout_console&, const utf8_string& string, Error_level error_level)
{
    std::cout << string.string() << std::endl;
}