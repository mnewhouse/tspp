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

#ifndef STATES_CUP_GUI_HPP
#define STATES_CUP_GUI_HPP

#include "cup/chat_message.hpp"

#include "client/client_interface.hpp"

namespace ts
{
    namespace resources
    {
        struct Resource_store;
    }

    namespace gui
    {
        class Context;
    }

    namespace states
    {
        class Car_selection_dialog;
        class Loading_progress_dialog;

        namespace impl
        {
            class Cup_GUI;
        }

        class Cup_GUI
        {
        public:
            Cup_GUI(const client::Client_interface* client_interface, gui::Context* context, const resources::Resource_store* resource_store);
            ~Cup_GUI();

            void show();
            void hide();

            void update(std::size_t frame_duration);
            void set_cup_state_text(utf8_string text);

            void show_car_selection_dialog();

            void show_progress_dialog();
            void hide_progress_dialog();

            void set_loading_progress(double progress);
            void set_loading_progress_text(utf8_string text);

            void output_chat_message(const cup::Composite_message& message);

            bool quit_event_pending() const;

        private:
            std::unique_ptr<impl::Cup_GUI> impl_;         
        };
    }
}

#endif