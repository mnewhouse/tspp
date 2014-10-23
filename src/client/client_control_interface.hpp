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


#ifndef CLIENT_CONTROL_INTERFACE_HPP
#define CLIENT_CONTROL_INTERFACE_HPP

#include "client_messages.hpp"

#include "controls/control_interface.hpp"
#include "controls/control.hpp"

namespace ts
{
    namespace action
    {
        class Stage;
    }

    namespace client
    {
        class Control_interface
            : public controls::Control_interface
        {
        public:
            Control_interface(const action::Stage* stage, const Message_center* local_message_center);

            virtual void handle_event(const controls::Control_event& event) override;
            virtual void update(std::size_t frame_duration) override;

        private:
            const Message_center* message_center_;
            const action::Stage* stage_;

            Server_message message_buffer_;
            std::vector<controls::Slot> updated_slots_;
        };
    }
}

#endif