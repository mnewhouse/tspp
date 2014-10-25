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

#include "client_messages.hpp"

namespace ts
{
    namespace resources
    {
        struct Resource_store;
        struct Track_identifier;
        struct Car_identifier;
    }

    namespace downloads
    {
        struct File_info;
    }

    namespace client
    {
        class Resource_downloader
            : public Message_listener
        {
        public:
            Resource_downloader(Message_center* message_center, resources::Resource_store* resource_store);
            ~Resource_downloader();

            void request_track(const resources::Track_identifier& track_identifier);
            void request_car(const resources::Car_identifier& car_identifier);

            virtual void handle_message(const Server_message& message) override;

            bool is_downloading() const;
            std::pair<std::size_t, std::size_t> download_progress() const;

        private:
            std::uint32_t generate_key() const;

            void handle_file_info_message(const Message& message);
            void handle_file_chunk_message(const Message& message);
            void handle_eof_message(const Message& message);
            void handle_finished_message(const Message& message);
            void handle_refusal_message(const Message& message);

            enum class Resource_type
            {
                Track,
                Car
            };

            struct Download_info
            {
                Resource_type resource_type;

                utf8_string target_directory_;
                utf8_string full_target_directory_;
                std::vector<downloads::File_info> file_info_;
                std::vector<std::vector<char>> file_buffers_;

                std::size_t current_file_ = 0;
                std::size_t total_size_ = 0;
                std::size_t bytes_downloaded_ = 0;
            };

            void save_resource_files(const Download_info& download_info);

            std::map<std::uint32_t, Download_info> download_map_;
            resources::Resource_store* resource_store_;
            const Message_center* message_center_;
            mutable std::mt19937 key_generator_;
            mutable Server_message message_buffer_;
        };
    }
}

#endif