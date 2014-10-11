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

#ifndef LOCAL_LOADING_SEQUENCE_HPP
#define LOCAL_LOADING_SEQUENCE_HPP

#include "action_loader.hpp"
#include "audio_loader.hpp"
#include "stage_loader.hpp"
#include "loaded_scene.hpp"

#include "cup/stage_data.hpp"

namespace ts
{
    namespace resources
    {
        struct Resource_store;
        class Track_handle;
    }

    namespace game
    {
        class Loading_sequence
        {
        public:
            Loading_sequence(const resources::Resource_store* resource_store);
            ~Loading_sequence() = default;

            void async_load(const action::Stage_data& stage_data);
            void poll();

            template <typename F>
            void set_completion_handler(F&& handler);

            template <typename F>
            void set_state_change_handler(F&& handler);

            utf8_string progress_string() const;
            double progress() const;

            Loaded_scene transfer_result();

        private:
            void test_readiness();
            void load_scene();
            void state_change(const utf8_string& new_state);

            Action_loader action_loader_;
            Audio_loader audio_loader_;
            Stage_loader stage_loader_;

            Loaded_scene loaded_scene_;
            action::Stage_data stage_data_;

            const resources::Resource_store* resource_store_;

            std::function<void()> completion_handler_;     
            std::function<void(const utf8_string& new_state)> state_change_handler_;
        };
    }
}

template <typename F>
void ts::game::Loading_sequence::set_completion_handler(F&& handler)
{
    completion_handler_ = handler;
}

template <typename F>
void ts::game::Loading_sequence::set_state_change_handler(F&& handler)
{
    state_change_handler_ = handler;
}

#endif