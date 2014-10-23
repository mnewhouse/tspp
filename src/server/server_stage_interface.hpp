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

#ifndef SERVER_STAGE_INTERFACE_HPP
#define SERVER_STAGE_INTERFACE_HPP

#include "server_messages.hpp"

namespace ts
{
    namespace cup
    {
        class Cup;
        struct Stage_data;
    }

    namespace game
    {
        class Stage_loader;
    }

    namespace action
    {
        class Stage;
    }

    namespace server
    {
        class Stage_interface
        {
        public:
            Stage_interface(Message_center* message_center, cup::Cup* cup);
            ~Stage_interface();

            const game::Stage_loader* async_load_stage(const cup::Stage_data& stage_data, std::function<void(const action::Stage*)> completion_callback);

            void update(std::size_t frame_duration);
            const action::Stage* stage() const;

            void clean_stage();
            void launch_action();

        private:
            class Impl;
            std::unique_ptr<Impl> impl_;
        };
    }
}

#endif