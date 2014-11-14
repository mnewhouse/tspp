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

#ifndef RESOURCE_DOWNLOAD_SERVER_HPP
#define RESOURCE_DOWNLOAD_SERVER_HPP

#include "server_messages.hpp"

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

       struct Resource_assets
       {
           Resource_assets();
           Resource_assets(Resource_assets&& other);
           Resource_assets& operator=(Resource_assets&& other);

           std::size_t resource_id_;
           std::vector<downloads::File_info> file_info_;
           std::vector<std::vector<std::uint8_t>> file_data_;
       };
    }

    namespace server
    {
        class Resource_download_server
            : public Message_listener
        {
        public:
            Resource_download_server(Message_center* message_center, const resources::Resource_store* resource_store);
            ~Resource_download_server();

            virtual void handle_message(const Client_message& message) override;

            void poll();

            void disconnect_client(const Generic_client& client);

        private:
            void handle_track_download_request(const Client_message& message);
            void handle_car_download_request(const Client_message& message);
            void handle_resource_download_request(const Client_message& message);
            void handle_pong_message(const Client_message& message);

            void resource_ready(const downloads::Resource_assets& assets);
            void deallocate_resource(std::size_t resource_id);

            void async_load_track_assets(std::size_t resource_id, const resources::Track_identifier& track_identifier);
            void async_load_car_assets(std::size_t resource_id, const resources::Car_identifier& car_identifier);
            void async_load_resource_assets(std::size_t resource_id, const utf8_string& resource_name);

            std::size_t allocate_resource_id() const;

            struct Download_info
            {
                std::uint32_t download_key_ = 0;
                std::size_t resource_id_ = 0;

                const std::vector<std::uint8_t>* file_ptr_ = nullptr;
                const std::vector<std::uint8_t>* file_end_ = nullptr;

                const std::uint8_t* data_ptr_ = nullptr;
                const std::uint8_t* data_end_ = nullptr;
            };

            void send_file_chunk(Download_info& download_info, const Generic_client& client);
            void send_eof_message(Download_info& download_info, const Generic_client& client);
            void send_finish_message(const Generic_client& client, std::uint32_t download_key);

            bool is_resource_being_downloaded(std::size_t resource_id) const;
            void refuse_download_request(std::size_t resource_id);

            using Resource_assets = downloads::Resource_assets;

            const resources::Resource_store* resource_store_;
            const Message_center* message_center_;
            std::unordered_map<Generic_client, std::vector<Download_info>> client_downloads_;

            std::map<std::size_t, Resource_assets> resource_assets_;
            std::map<std::size_t, std::future<Resource_assets>> loading_resources_;

            struct Concrete_resources;
            struct Car_resource;
            struct Track_resource;
            struct Script_resource;

            std::unique_ptr<Concrete_resources> concrete_resources_;
            
            std::size_t current_resource_id_ = 0;
        };
    }
}

#endif