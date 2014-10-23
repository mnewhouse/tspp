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
#include "client_control_interface.hpp"

#include "action/stage.hpp"
#include "action/action_messages.hpp"

#include "controls/control_event.hpp"
#include "controls/control_center.hpp"
#include "controls/controllable.hpp"

ts::client::Control_interface::Control_interface(const action::Stage* stage, const Message_center* client_message_center)
: message_center_(client_message_center),
  stage_(stage)
{
}

void ts::client::Control_interface::handle_event(const controls::Control_event& event)
{
    message_buffer_.message_type = Message_type::Fast;

    const auto& control_center = stage_->control_center();
    control_center.handle_control_event(event);

    if (std::find(updated_slots_.begin(), updated_slots_.end(), event.slot) == updated_slots_.end())
    {
        updated_slots_.push_back(event.slot);
    }
}

void ts::client::Control_interface::update(std::size_t frame_duration)
{
    const auto& control_center = stage_->control_center();

    for (auto slot : updated_slots_)
    {
        const auto& controllables = control_center.get_controllables_by_slot(slot);
        for (auto controllable : controllables)
        {
            message_buffer_.message = action::make_control_event_message(controllable->controllable_id(), controllable->control_state_mask(),
                                                                     stage_->stage_time());
            message_center_->dispatch_message(message_buffer_);
            message_center_->handle_message(message_buffer_);
        }
    }

    updated_slots_.clear();
}