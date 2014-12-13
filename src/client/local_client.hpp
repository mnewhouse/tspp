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

#ifndef LOCAL_CLIENT_HPP
#define LOCAL_CLIENT_HPP

#include "client_messages.hpp"

namespace ts
{
    namespace server
    {
        class Server;
        struct Client_message;
    }

    namespace cup
    {
        class Chatbox;
        struct Chatbox_listener;
    }

    namespace resources
    {
        struct Resource_store;
        class Loading_interface;
    }

    namespace controls
    {
        class Control_interface;
    }

    namespace action
    {
        class Stage;
    }

    namespace scene
    {
        struct Scene;
    }

    namespace gui
    {
        class Context;
    }

    namespace client
    {
        class Client_interface;

        class Local_client
        {
        public:
            Local_client(server::Server* server, resources::Resource_store* resource_store);
            ~Local_client();

            void update(std::size_t frame_duration);

            const Client_interface* client_interface() const;

            // This returns a non-owning pointer.
            std::unique_ptr<controls::Control_interface> make_control_interface(const action::Stage* stage) const;
            scene::Scene acquire_scene();

            using Scene_completion_callback = std::function<void()>;

            // Ditto.
            const resources::Loading_interface* async_load_scene(const action::Stage* stage, Scene_completion_callback);

        private:
            class Impl;
            std::unique_ptr<Impl> impl_;

            friend class Local_client_link;
        };

        class Local_client_link
        {
        public:
            Local_client_link(const Local_client* local_client);

            void operator()(const server::Client_message& client_message) const;

        private:
            const Local_client* local_client_;
            mutable Server_message message_buffer_;
        };
    }
}

#endif