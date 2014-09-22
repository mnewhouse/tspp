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

#ifndef GUI_EVENT_MANAGER_HPP
#define GUI_EVENT_MANAGER_HPP

namespace ts
{
    namespace gui
    {
        // Event traits template.
        // This must be specialized for concrete event types with function
        // signatures different from the default, being `void()`;
        template <typename EventType>
        struct Event_traits
        {
            using function_type = void();
        };

        // Base class for event handlers.
        // This doesn't have to do all that much, it merely has make sure
        // derived classes are destructed properly.
        struct Event_handler_base
        {
            virtual ~Event_handler_base() = default;
        };

        // Class template that represents a stored event handler.
        // It doesn't have a public interface, it's merely to allow
        // for the removal of specific event handlers.
        template <typename EventType>
        struct Event_handler
        {
            Event_handler() = default;

        private:
            friend class Event_manager;

            Event_handler(Event_handler_base* event_handler)
                : event_handler_(event_handler)
            {}

            Event_handler_base* event_handler_ = nullptr;
        };

        // Class template that simply represents a callable function
        // deriving from a common base.
        template <typename FuncType>
        struct Event_handler_model
            : public Event_handler_base
        {
        public:
            template <typename F>
            Event_handler_model(F func)
                : func_(func)
            {}

            template <typename... Args>
            void operator()(Args&&... args)
            {
                func_(std::forward<Args>(args)...);
            }

        private:
            std::function<FuncType> func_;
        };

        // The event manager class allows for registering and triggering
        // of, well, events. Every event type is in fact a C++ type, and 
        // with some minor template magic this is a compile-time type-safe solution.
        class Event_manager
        {
        public:
            Event_manager() = default;

            // Triggers an event of a given type, and forwards the arguments to
            // the registered event handlers.
            template <typename EventType, typename... Args>
            void trigger_event(EventType event_type, Args&&... args)
            {
                using event_t = typename std::decay<EventType>::type;
                auto id = type_id<event_t>();

                auto it = event_handler_map_.find(id);
                if (it == event_handler_map_.end())
                {
                    return;
                }

                auto& list = it->second;

                using callback_type = typename Event_traits<event_t>::function_type;
                using handler_type = Event_handler_model<callback_type>;

                for (const auto& handler : list)
                {
                    auto concrete_handler = dynamic_cast<handler_type*>(&*handler);

                    if (concrete_handler)
                    {
                        (*concrete_handler)(std::forward<Args>(args)...);
                    }
                }
            }

            // Registers an event handler with this instance, meaning the callable that was passed
            // will be invoked whenever an event of the given type is fired.
            // It returns a handle that can be used to remove the event handler.
            template <typename EventType, typename FuncType>
            Event_handler<typename std::decay<EventType>::type> add_event_handler(EventType event_type, FuncType func)
            {
                using event_t = typename std::decay<EventType>::type;

                auto id = type_id<event_t>();
                auto& handler_list = event_handler_map_[id];

                using callback_type = typename Event_traits<event_t>::function_type;
                using handler_type = Event_handler_model<callback_type>;

                handler_list.emplace_back(std::make_unique<handler_type>(func));
                

                return Event_handler<event_t>(&*handler_list.back());
            }

            // Removes an event handler that was previously returned by add_event_handler, 
            // and performs all the necessary cleanup.
            template <typename EventType>
            void remove_event_handler(Event_handler<EventType> event_handler)
            {
                using event_t = typename std::decay<EventType>::type;

                auto id = type_id<event_t>();
                auto& handler_list = event_handler_map_[id];

                auto ptr = event_handler.event_handler_;
                auto it = std::find_if(handler_list.begin(), handler_list.end(), 
                                       [ptr](const std::unique_ptr<Event_handler_base>& handler)
                {
                    return &*handler == ptr;
                });

                if (it != handler_list.end())
                {
                    handler_list.erase(it);
                }
            }

        private:
            using Event_handler_list = std::vector<std::unique_ptr<Event_handler_base>>;

            std::map<type_id_type, Event_handler_list> event_handler_map_;
        };
    }
}

#endif