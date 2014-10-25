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

#include "resources/track_identifier.hpp"
#include "resources/car_identifier.hpp"

namespace ts
{
    namespace downloads
    {
        using messages::Message;

        struct Message_type
        {
            // Client -> server
            static const std::uint32_t track_download_request = 3401;
            static const std::uint32_t car_download_request = 3411;
            static const std::uint32_t resource_download_request = 3421;

            static const std::uint32_t pong = 3477;


            // Server -> client
            static const std::uint32_t download_refused = 4473;

            static const std::uint32_t file_info = 4481;
            static const std::uint32_t eof = 4482;
            static const std::uint32_t finished = 4483;

            static const std::uint32_t file_chunk = 4497;
        };

        enum class Resource_type
        {
            Track,
            Car,
            Module,
        };


        // Downloads
        struct Track_download_request
        {
            std::uint32_t download_key = 0;
            resources::Track_identifier track_identifier;
        };

        struct Car_download_request
        {
            std::uint32_t download_key = 0;
            resources::Car_identifier car_identifier;
        };

        struct File_info
        {
            utf8_string file_name;
            std::size_t file_size;
        };

        struct File_list
        {
            std::uint32_t download_key = 0;
            std::vector<File_info> file_info;
        };

        struct File_chunk
        {
            std::uint32_t download_key = 0;
            std::vector<std::uint8_t> chunk;
        };

        struct Download_message
        {
            std::uint32_t download_key = 0;
        };

        Message make_track_download_request(std::uint32_t download_key, const resources::Track_identifier& track_identifier);
        Track_download_request parse_track_download_request(const Message& message);

        Message make_car_download_request(std::uint32_t download_key, const resources::Car_identifier& car_identifier);
        Car_download_request parse_car_download_request(const Message& message);

        Message make_pong_message(std::uint32_t download_key);
        Download_message parse_pong_message(const Message& message);


        // Server->client download messages
        Message make_file_info_message(std::uint32_t download_key, const std::vector<File_info>& file_info);
        File_list parse_file_info_message(const Message& message);

        Message make_download_refusal_message(std::uint32_t download_key);
        Download_message parse_download_refusal_message(const Message& message);

        Message make_file_chunk_message(std::uint32_t download_key, const std::uint8_t* data, std::uint32_t data_size);
        File_chunk parse_file_chunk_message(const Message& message);

        Message make_eof_message(std::uint32_t download_key);
        Download_message parse_eof_message(const Message& message);

        Message make_finished_message(std::uint32_t download_key);
        Download_message parse_finished_message(const Message& message);
    }
}

#endif