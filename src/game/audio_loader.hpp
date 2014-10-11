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

#ifndef AUDIO_LOADER_HPP
#define AUDIO_LOADER_HPP

#include "generic_loader.hpp"

#include "audio/sound_controller.hpp"


namespace ts
{
    namespace action
    {
        class Stage;
    }

    namespace game
    {
        enum class Audio_loader_state
        {
            None,
            Loading_audio
        };

        class Audio_loader
            : public Generic_loader<Audio_loader_state, std::unique_ptr<audio::Sound_controller>>
        {
        public:
            void async_load(const action::Stage* stage, const resources::Audio_settings& audio_settings);

        private:
            std::unique_ptr<audio::Sound_controller> load_audio(const action::Stage* stage, const resources::Audio_settings& audio_setting);
        };
    }
}

#endif