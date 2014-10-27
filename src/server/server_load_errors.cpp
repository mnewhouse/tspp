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
#include "server_load_errors.hpp"

#include "cup/cup_messages.hpp"
#include "cup/cup_controller.hpp"

void ts::server::handle_load_error(const utf8_string& error_string, const Message_center& message_center, cup::Cup_controller& cup_controller)
{
    // Display a message for local client
    Client_message out_message;
    out_message.client = Generic_client(local_client);
    out_message.message = cup::make_load_error_message(error_string);
    message_center.dispatch_message(out_message);

    // And skip the stage
    cup_controller.skip_stage();
}