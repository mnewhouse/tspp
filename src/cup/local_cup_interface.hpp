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

#ifndef LOCAL_CUP_INTERFACE_HPP
#define LOCAL_CUP_INTERFACE_HPP

#include "cup_interface.hpp"

namespace ts
{
    namespace cup
    {
        class Local_cup_interface
            : public Cup_interface
        {
        public:
            Local_cup_interface(Cup* cup);

            virtual void select_cars(const std::vector<Car_selection>& car_selection) override;
            virtual void signal_ready() override;
            virtual void write_chat_message(const utf8_string& message) override;

        private:
            std::unordered_set<Player_handle> ready_local_players_;

        };
    }
}

#endif