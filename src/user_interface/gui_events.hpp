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

#ifndef GUI_EVENTS_HPP
#define GUI_EVENTS_HPP

#include "core/utility.hpp"

#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>

namespace ts
{
    namespace gui
    {
        template <typename EventType>
        struct Event_traits;

        class Event_dispatcher_base
        {
        public:
            virtual ~Event_dispatcher_base() = 0 {}
        };

        template <typename EventType>
        class Event_dispatcher
            : public gui::Event_dispatcher_base
        {
        public:
            using function_type = typename Event_traits<EventType>::function_type;

            template <typename F>
            void add_event_handler(F func);

            template <typename... Args>
            void dispatch(Args&&... args);

        private:
            std::vector<std::function<function_type>> event_handlers_;
        };


        class Event_manager
        {
        public:
            template <typename EventType, typename F>
            void add_event_handler(EventType event_type, F function);

            template <typename EventType, typename... Args>
            void trigger_event(EventType event_type, Args&&... args);

        private:
            using event_type_id = void*;
            std::unordered_map<event_type_id, std::unique_ptr<Event_dispatcher_base>> event_dispatchers_;
        };
    }
}

template <typename EventType, typename F>
void ts::gui::Event_manager::add_event_handler(EventType, F function)
{
    auto id = type_id<EventType>();
    auto it = event_dispatchers_.find(id);
    if (it == event_dispatchers_.end())
    {
        auto new_dispatcher = std::make_unique<Event_dispatcher<EventType>>();
        new_dispatcher->add_event_handler(function);

        auto result = event_dispatchers_.insert(std::make_pair(id, std::move(new_dispatcher)));
    }

    else
    {
        auto dispatcher = dynamic_cast<Event_dispatcher<EventType>*>(&*it->second);
        dispatcher->add_event_handler(function);
    }
}

template <typename EventType, typename... Args>
void ts::gui::Event_manager::trigger_event(EventType event_type, Args&&... args)
{
    auto id = type_id<EventType>();
    auto it = event_dispatchers_.find(id);
    if (it != event_dispatchers_.end())
    {
        auto dispatcher = dynamic_cast<Event_dispatcher<EventType>*>(&*it->second);
        dispatcher->dispatch(std::forward<Args>(args)...);
    }
}

template <typename EventType>
template <typename F>
void ts::gui::Event_dispatcher<EventType>::add_event_handler(F function)
{
    event_handlers_.emplace_back(function);
}

template <typename EventType>
template <typename... Args>
void ts::gui::Event_dispatcher<EventType>::dispatch(Args&&... args)
{
    for (const auto& func : event_handlers_)
    {
        func(std::forward<Args>(args)...);
    }
}

#endif