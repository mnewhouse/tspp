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

#ifndef CLIENT_STAGE_CONDUCTOR_HPP
#define CLIENT_STAGE_CONDUCTOR_HPP

#include "client_messages.hpp"

#include "resources/settings_copy.hpp"
#include "resources/settings/network_settings.hpp"

namespace ts
{
    namespace action
    {
        class Stage;
        class Stage_conductor;
        struct Game_state_message;
    }

    namespace client
    {
        class Stage_conductor
            : public Scoped_message_listener
        {
        public:
            Stage_conductor(Message_center* message_center, action::Stage* stage, 
                            resources::Network_settings* network_settings);
            ~Stage_conductor();

            void update(std::size_t frame_duration);
            void purge_old_events(std::uint32_t time_point);

        private:
            virtual void handle_message(const Server_message& server_message) override;

            void handle_control_event_message(const Message& message);
            void handle_game_state_message(const Message& message);

            std::unique_ptr<action::Stage_conductor> stage_conductor_;
            std::vector<action::Game_state_message> queued_game_state_;

            template <typename T>
            using Settings_copy = resources::Settings_copy<T>;
            using Network_settings = resources::Network_settings;

            Settings_copy<Network_settings> network_settings_;
            std::int32_t advance_time_ = 100;
        };
    }
}

#endif