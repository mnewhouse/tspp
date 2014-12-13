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
#include "control_event_processor.hpp"
#include "control_interface.hpp"

ts::controls::Event_processor::Event_processor(std::unique_ptr<Control_interface> control_interface, const Key_mapping& key_mapping)
    : control_interface_(std::move(control_interface)),
      event_translator_(key_mapping)
{
}

ts::controls::Event_processor::~Event_processor()
{
}

void ts::controls::Event_processor::handle_event(const sf::Event& event)
{
    event_translator_.enqueue_event(event);

    controls::Control_event control_event;
    while (event_translator_.poll_event(control_event))
    {
        control_interface_->handle_event(control_event);
    }    
}