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
#include "event_manager.hpp"

ts::script::Event_handler::Event_handler(utf8_string event_name, std::list<Function>::iterator list_it)
: event_name_(std::move(event_name)),
  function_it_(list_it)
{
}

ts::script::Event_handler ts::script::Event_manager::register_event_handler(const utf8_string& event_name, 
                                                                                           const Function& function)
{
    auto& handler_list = event_handlers_[event_name];
    auto function_it = handler_list.insert(handler_list.end(), function);

    return Event_handler(event_name, function_it);
}

void ts::script::Event_manager::remove_event_handler(const Event_handler& event_handler)
{
    auto map_it = event_handlers_.find(event_handler.event_name_);
    if (map_it != event_handlers_.end())
    {
        map_it->second.erase(event_handler.function_it_);
    }
}
