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
#include "event_dispatcher.hpp"
#include "event_handler.hpp"

ts::Generic_scope_exit ts::game::Event_dispatcher::acquire_event_hog(Event_handler* hogger)
{
    event_hogger_ = hogger;

    return Generic_scope_exit([this, hogger]() { release_event_hog(hogger); });
}

void ts::game::Event_dispatcher::release_event_hog(Event_handler* hogger)
{
    if (hogger == event_hogger_)
    {
        event_hogger_ = nullptr;
    }
}

void ts::game::Event_dispatcher::handle_event(const sf::Event& event)
{
    if (event_hogger_ == nullptr)
    {
        call_listeners(&Event_handler::handle_event, event);
    }

    else
    {
        event_hogger_->handle_event(event);
    }
}

bool ts::game::Event_dispatcher::hogged() const
{
    return event_hogger_ != nullptr;
}