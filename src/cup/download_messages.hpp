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

#ifndef DOWNLOAD_MESSAGES_HPP
#define DOWNLOAD_MESSAGES_HPP

#include "messages/message.hpp"

#include "resources/track_definition.hpp"

namespace ts
{
    namespace downloads
    {
        using messages::Message;

        class Message_type
        {
            // Client -> server
            static const std::uint32_t track_download_request = 3401;
            static const std::uint32_t car_download_request = 3411;
            static const std::uint32_t resource_download_request = 3421;

            static const std::uint32_t pong = 3477;


            // Server -> client
            static const std::uint32_t file_info = 4481;
            static const std::uint32_t eof = 4482;
            static const std::uint32_t finished = 4483;

            static const std::uint32_t file_chunk = 4497;
        };


        // Downloads
        struct Track_download_request
        {
            std::uint32_t message_type = 0;
            std::uint32_t download_key = 0;
            resources::Track_definition track_definition;
        };

        struct File_info
        {
            utf8_string file_name;
            std::size_t file_size;
        };

        struct File_list
        {
            std::uint32_t message_type = 0;
            std::uint32_t download_key = 0;
            std::vector<File_info> file_info;
        };

        struct File_chunk
        {
            std::uint32_t message_type = 0;
            std::uint32_t downoad_key = 0;
            std::vector<std::uint8_t> chunk;
        };

        Message make_track_download_request(std::uint32_t download_key, const resources::Track_definition& track_definition);
        Track_download_request parse_track_download_request(const Message& message);

        Message make_pong_message(std::uint32_t download_key);

        // Server->client download messages
        Message make_file_info_message(std::uint32_t download_key, const std::vector<File_info>& file_info);
        File_list parse_file_info_message(const Message& message);

        Message make_file_chunk_message(std::uint32_t download_key, const std::uint8_t* data, const std::uint8_t* data_end);
        File_chunk parse_file_chunk_message(const Message& message);

        Message make_eof_message(std::uint32_t download_key);
        Message make_finished_message(std::uint32_t download_key);        
    }
}

#endif