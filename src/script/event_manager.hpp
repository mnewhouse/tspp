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

#ifndef SCRIPT_EVENT_MANAGER_HPP
#define SCRIPT_EVENT_MANAGER_HPP

#include "script_function.hpp"

namespace ts
{
    namespace script
    {
        struct Event_handler
        {
        private:
            friend class Event_manager;
            
            Event_handler(utf8_string event_name, std::list<Function>::iterator function_it);
            
            utf8_string event_name_;
            std::list<Function>::iterator function_it_;
        };

        class Event_manager
        {
        public:
            template <typename... Args>
            void trigger_event(const utf8_string& event_name, Args&&... args);

            Event_handler register_event_handler(const utf8_string& event_name, const Function& function);
            void remove_event_handler(const Event_handler& event_handler);

        private:
            std::map<utf8_string, std::list<Function>> event_handlers_;
        };
    }
}

template <typename... Args>
void ts::script::Event_manager::trigger_event(const utf8_string& event_name, Args&&... args)
{
    auto list_it = event_handlers_.find(event_name);
    if (list_it != event_handlers_.end())
    {
        for (const auto& function : list_it->second)
        {
            function(std::forward<Args>(args)...);
        }
    }
}

#endif