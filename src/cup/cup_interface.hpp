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

#ifndef CUP_INTERFACE_HPP
#define CUP_INTERFACE_HPP

#include "cup_metadata.hpp"
#include "cup_listener.hpp"
#include "chatbox.hpp"

#include "resources/settings/car_settings.hpp"
#include "resources/settings/track_settings.hpp"

namespace ts
{
    namespace cup
    {
        class Cup;

        struct Car_selection
        {
            Player_handle player_handle;
            resources::Car_handle car_handle;
            std::uint32_t car_index;
        };

        class Cup_interface
            : private Cup_listener
        {
        public:
            Cup_interface(Cup* cup);

            void advance();
            void end_cup();

            void set_cup_state(Cup_state cup_state);
            void set_cup_progress(std::size_t progress);

            const Cup* cup() const;
            Chatbox* chatbox();

            void clear_players();
            Player_handle add_player(const Player& player);
            Player_handle add_player(const Player& player, Player_id handle);

            void clear_tracks();
            void add_track(const resources::Track_handle& track_handle);

            void select_car(const resources::Car_handle& car_handle);
            void set_car_mode(resources::Car_mode car_mode);            
            
            virtual void select_cars(const std::vector<Car_selection>& car_selection) = 0;
            virtual void signal_ready() = 0;
            virtual void write_chat_message(const utf8_string& message) = 0;

        protected:
            void output_chat_message(const Composite_message& message);
            void set_player_car(Player_handle player_handle, resources::Car_handle car_handle);

        private:
            Cup* cup_;

            Chatbox chatbox_;
        };
    }
}

#endif