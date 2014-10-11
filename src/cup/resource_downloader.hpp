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

#ifndef RESOURCE_DOWNLOADER_HPP
#define RESOURCE_DOWNLOADER_HPP

#include "resources/track_definition.hpp"

namespace ts
{
    namespace network
    {
        class Client;
        class Message;
    }

    namespace cup
    {
        enum class Resource_type
        {
            Track,
            Car,
            Module,
        };

        class Resource_downloader
        {
        public:
            Resource_downloader(network::Client* client);

            void request_track(const resources::Track_definition& track_definition);

            void handle_download_message(const network::Message& message);

            bool is_downloading() const;

        private:
            std::uint32_t generate_key() const;

            struct Download_information
            {
                std::vector<char> file_buffer_;
            };

            std::map<std::uint32_t, Download_information> download_map_;

            network::Client* client_;
        };
    }
}

#endif