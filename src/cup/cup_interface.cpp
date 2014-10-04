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
#include "cup_interface.hpp"
#include "cup.hpp"

ts::cup::Cup_interface::Cup_interface(Cup* cup)
  : cup_(cup)
{
    cup_->add_cup_listener(this);
}

const ts::cup::Cup* ts::cup::Cup_interface::cup() const
{
    return cup_;
}

ts::cup::Chatbox* ts::cup::Cup_interface::chatbox()
{
    return &chatbox_;
}

void ts::cup::Cup_interface::advance()
{
    cup_->advance();
}

void ts::cup::Cup_interface::end_cup()
{
    cup_->end();
}

ts::cup::Player_handle ts::cup::Cup_interface::add_player(const Player& player)
{
    return cup_->add_player(player);
}

void ts::cup::Cup_interface::set_player_car(Player_handle player_handle, resources::Car_handle car_handle)
{
    cup_->set_player_car(player_handle, car_handle);
}

void ts::cup::Cup_interface::output_chat_message(const Composite_message& message)
{
    chatbox_.output_message(message);
}

void ts::cup::Cup_interface::set_cup_state(Cup_state state)
{
    cup_->set_cup_state(state);
}

void ts::cup::Cup_interface::set_cup_progress(std::size_t progress)
{
    cup_->set_cup_progress(progress);
}