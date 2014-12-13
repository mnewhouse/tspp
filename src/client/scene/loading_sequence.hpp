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

#ifndef CLIENT_LOADING_SEQUENCE_HPP
#define CLIENT_LOADING_SEQUENCE_HPP

#include "scene.hpp"

#include "scene_loader.hpp"
#include "audio_loader.hpp"

namespace ts
{
    namespace resources
    {
        struct Resource_store;
        class Track_handle;
    }

    namespace action
    {
        class Stage_interface;
    }

    namespace cup
    {
        struct Stage_data;
    }

    namespace scene
    {
        class Loading_sequence
            : public resources::Loading_interface
        {
        public:
            Loading_sequence(const resources::Resource_store* resource_store);
            ~Loading_sequence() = default;

            void async_load(const cup::Stage_data& stage_data, action::Stage_interface* stage_interface);
            void async_load(const action::Stage* stage);
            void poll();

            template <typename F>
            void set_completion_handler(F&& handler);

            template <typename F>
            void set_state_change_handler(F&& handler);

            virtual utf8_string progress_string() const override;

            Scene transfer_result();

        private:
            void load_scripts_if_ready();
            void load_scene();

            void handle_completion();
            void state_change(const utf8_string& new_state);

            Scene_loader scene_loader_;
            Audio_loader audio_loader_;

            Scene loaded_scene_;

            const action::Stage* stage_ = nullptr;
            const resources::Loading_interface* stage_loader_ = nullptr;

            const resources::Resource_store* resource_store_;

            std::function<void()> completion_handler_;     
            std::function<void(const utf8_string& new_state)> state_change_handler_;

            std::exception_ptr exception_ptr_;
        };
    }
}

template <typename F>
void ts::scene::Loading_sequence::set_completion_handler(F&& handler)
{
    completion_handler_ = handler;
}

template <typename F>
void ts::scene::Loading_sequence::set_state_change_handler(F&& handler)
{
    state_change_handler_ = handler;
}

#endif