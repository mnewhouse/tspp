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

#ifndef CONTROL_EVENT_TRANSLATOR_HPP
#define CONTROL_EVENT_TRANSLATOR_HPP

#include "key_mapping.hpp"
#include "control_event.hpp"

namespace ts
{
    namespace controls
    {
        class Event_translator
        {
        public:
            Event_translator(const Key_mapping& key_mapping);

            void enqueue_event(const sf::Event& event);
            void enqueue_event(const Control_event& event);

            bool poll_event(Control_event& event);

        private:
            std::deque<Control_event> event_queue_;
            Key_mapping key_mapping_;
        };
    }
}

#endif