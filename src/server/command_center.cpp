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
#include "command_center.hpp"

namespace ts
{
    namespace server
    {
        std::pair<ts::utf8_string, ts::utf8_string> split_command_string(const utf8_string& string);
    }
}

std::pair<ts::utf8_string, ts::utf8_string> ts::server::split_command_string(const utf8_string& string)
{
    auto cmd_start = std::next(string.begin());
    auto cmd_end = std::find_if(cmd_start, string.end(), boost::is_space());
    auto result = std::make_pair(utf8_string(), utf8_string());

    boost::to_lower_copy(std::back_inserter(result.first), boost::make_iterator_range(cmd_start, cmd_end));

    auto params_start = std::find_if_not(cmd_end, string.end(), boost::is_space());
    result.second = utf8_string(params_start, string.end());

    return result;
}

bool ts::server::Command_center::has_command_prefix(const utf8_string& message) const
{
    return !message.empty() && *message.begin() == '/';
}

void ts::server::Command_center::handle_client_message(const Generic_client& client, const utf8_string& message) const
{
    if (has_command_prefix(message))
    {
        auto tokenized_command = split_command_string(message);
        const auto& command_name = tokenized_command.first;
        const auto& parameters = tokenized_command.second;

        auto it = command_map_.find(command_name);
        if (it != command_map_.end())
        {
            for (const auto& command_handler : it->second)
            {
                command_handler(client, command_name, parameters);
            }
        }
    }
}

ts::server::Command_center::Command_handle ts::server::Command_center::add_command_handler(const utf8_string& command_name, 
                                                                                           Command_handler handler)
{
    utf8_string lc_command_name;
    boost::to_lower_copy(std::back_inserter(lc_command_name), command_name);

    auto map_it = command_map_.find(lc_command_name);
    if (map_it == command_map_.end())
    {
        auto emplace_result = command_map_.emplace(std::piecewise_construct, std::forward_as_tuple(lc_command_name), std::forward_as_tuple());
        map_it = emplace_result.first;
    }

    map_it->second.push_back(std::move(handler));
    return Command_handle(this, &map_it->first, std::prev(map_it->second.end()));
}

void ts::server::Command_center::remove_command_handler(Command_handle handle)
{
    auto map_it = command_map_.find(*handle.command_name_);
    if (map_it != command_map_.end())
    {
        map_it->second.erase(handle.it_);

        // Invalidate state, to prevent double deletion
        handle.command_name_ = nullptr;
        handle.command_center_ = nullptr;
        handle.it_ = {};
    }
}

ts::server::Command_center::Command_handle::Command_handle()
{
}

ts::server::Command_center::Command_handle::Command_handle(Command_center* command_center, const utf8_string* command_name,
                                                            std::list<Command_handler>::iterator it)
: command_center_(command_center),
  command_name_(command_name),
  it_(it)
{
}

ts::server::Command_center::Command_handle::~Command_handle()
{
    if (command_center_)
    {
        auto command_center = command_center_;
        command_center->remove_command_handler(std::move(*this));
    }
}

ts::server::Command_center::Command_handle::Command_handle(Command_handle&& other)
: command_center_(other.command_center_),
  command_name_(other.command_name_),
  it_(other.it_)
{
    other.command_center_ = nullptr;
    other.command_name_ = nullptr;
    other.it_ = {};
}

ts::server::Command_center::Command_handle& ts::server::Command_center::Command_handle::operator=(Command_handle&& other)
{
    std::swap(command_center_, other.command_center_);
    std::swap(command_name_, other.command_name_);
    std::swap(it_, other.it_);

    return *this;
}

ts::server::Command_center::Command_handle::operator bool() const
{
    return command_center_ != nullptr && command_name_ != nullptr;
}