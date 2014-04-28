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

#ifndef AUDIO_AUDIO_STORE_HPP
#define AUDIO_AUDIO_STORE_HPP

#include <map>
#include <memory>

#include <SFML/Audio.hpp>

namespace ts
{
    namespace audio
    {
        using Audio_handle = std::shared_ptr<sf::SoundBuffer>;
        struct Load_error
            : std::exception
        {
            Load_error(const std::string& file_name) {}
        };

        class Audio_store
        {
        public:
            Audio_store(std::string search_directory = "");

            Audio_handle operator[](std::string file_name);

        private:
            std::string search_directory_;
            std::map<std::string, Audio_handle> loaded_samples_;
            
        };
    }
}

#endif