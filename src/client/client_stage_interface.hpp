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

#ifndef CLIENT_STAGE_INTERFACE_HPP
#define CLIENT_STAGE_INTERFACE_HPP

#include "client_messages.hpp"

namespace ts
{
    namespace cup
    {
        struct Stage_data;
    }

    namespace action
    {
        class Stage;
    }

    namespace game
    {
        class Stage_loader;
    }

    namespace resources
    {
        class Network_settings;
    }

    namespace client
    {
        class Stage_interface
        {
        public:
            Stage_interface(Message_center* message_center, resources::Network_settings* network_settings);
            ~Stage_interface();

            const game::Stage_loader* async_load_stage(const cup::Stage_data& stage_data, std::function<void(const action::Stage*)> completion_callback);

            void update(std::size_t frame_duration);

            void poll_loader();
            void clean_stage();
            void launch_action();

            const action::Stage* stage() const;

        private:
            class Impl;
            std::unique_ptr<Impl> impl_;
        };
    }
}

#endif