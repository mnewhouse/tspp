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

#include "stdinc.hpp"
#include "resource_downloader.hpp"

#include "cup/cup_messages.hpp"
#include "cup/download_messages.hpp"
#include "cup/chat_message.hpp"

#include "resources/resource_store.hpp"
#include "resources/car_store.hpp"
#include "resources/track_store.hpp"


ts::client::Resource_downloader::Resource_downloader(Message_center* message_center, resources::Resource_store* resource_store)
: Message_listener(message_center),
  message_center_(message_center), 
  resource_store_(resource_store)
{
}

ts::client::Resource_downloader::~Resource_downloader()
{
}

std::uint32_t ts::client::Resource_downloader::generate_key() const
{
    std::uint32_t key = key_generator_();
    while (download_map_.find(key) != download_map_.end())
    {
        key = key_generator_();
    }

    return key;
}

void ts::client::Resource_downloader::handle_message(const Server_message& server_message)
{
    if (server_message.message_type == Message_type::Reliable)
    {
        const auto& message = server_message.message;
        using Msg = downloads::Message_type;
        switch (message.type())
        {
        case Msg::download_refused:
            handle_refusal_message(message);
            break;

        case Msg::file_info:
            handle_file_info_message(message);
            break;

        case Msg::file_chunk:
            handle_file_chunk_message(message);
            break;

        case Msg::eof:
            handle_eof_message(message);
            break;

        case Msg::finished:
            handle_finished_message(message);
        }
    }
}

void ts::client::Resource_downloader::handle_file_info_message(const Message& message)
{
    auto file_list = downloads::parse_file_info_message(message);
    auto map_it = download_map_.find(file_list.download_key);
    if (map_it != download_map_.end())
    {
        auto& download_info = map_it->second;
        download_info.file_info_ = std::move(file_list.file_info);
        download_info.bytes_downloaded_ = 0;
        download_info.total_size_ = 0;
        download_info.current_file_ = 0;
        download_info.total_size_ = 0;

        const auto& file_info = download_info.file_info_;
        for (auto& file : file_info)
        {
            download_info.total_size_ += file.file_size;
        }

        auto file_count = file_info.size();

        cup::Composite_message chat_message;
        chat_message.append("About to receive ", sf::Color(255, 150, 0));
        chat_message.append(std::to_string(file_count), sf::Color(255, 0, 0));
        if (download_info.resource_type == Resource_type::Track)
        {
            chat_message.append(" track", sf::Color(255, 150, 0));
        }

        else if (download_info.resource_type == Resource_type::Car)
        {
            chat_message.append(" car", sf::Color(255, 150, 0));
        }

        chat_message.append(" files.", sf::Color(255, 150, 0));

        message_buffer_.message = cup::make_chatbox_output_message(chat_message);
        message_center_->handle_message(message_buffer_);

        
        message_buffer_.message = downloads::make_pong_message(file_list.download_key);
        message_center_->dispatch_message(message_buffer_);
    }
}

void ts::client::Resource_downloader::handle_refusal_message(const Message& message)
{
    auto refusal = downloads::parse_download_refusal_message(message);
    download_map_.erase(refusal.download_key);

    // Download refused.
    cup::Composite_message chat_message;
    chat_message.append("Error: download failed.", sf::Color(200, 0, 0));

    message_buffer_.message = cup::make_chatbox_output_message(chat_message);    
    message_center_->handle_message(message_buffer_);
}

void ts::client::Resource_downloader::handle_file_chunk_message(const Message& message)
{
    auto file_chunk = downloads::parse_file_chunk_message(message);
    auto map_it = download_map_.find(file_chunk.download_key);
    if (map_it != download_map_.end())
    {
        auto& download_info = map_it->second;

        // If there's no buffer for this file yet, create it
        if (download_info.current_file_ >= download_info.file_buffers_.size())
        {
            download_info.file_buffers_.resize(download_info.current_file_ + 1);
        }

        // Insert the chunk into the buffer and increment the downloaded byte count
        auto& buffer = download_info.file_buffers_[download_info.current_file_];
        buffer.insert(buffer.end(), file_chunk.chunk.begin(), file_chunk.chunk.end());

        download_info.bytes_downloaded_ += file_chunk.chunk.size();

        // Inform the server that we've received the chunk.
        message_buffer_.message = downloads::make_pong_message(file_chunk.download_key);
        message_center_->dispatch_message(message_buffer_);
    }
}

void ts::client::Resource_downloader::handle_eof_message(const Message& message)
{
    auto eof = downloads::parse_eof_message(message);
    auto map_it = download_map_.find(eof.download_key);
    if (map_it != download_map_.end())
    {
        auto& download_info = map_it->second;
        const auto& file_name = download_info.file_info_[download_info.current_file_].file_name;

        // Increment file index
        ++download_info.current_file_;

       cup::Composite_message chat_message;
       chat_message.append("Downloaded file ", sf::Color(255, 150, 0));
       chat_message.append(file_name, sf::Color(255, 220, 50));
       chat_message.append(".", sf::Color(255, 150, 0));

       message_buffer_.message = cup::make_chatbox_output_message(chat_message);
       message_center_->handle_message(message_buffer_);
    }
}

void ts::client::Resource_downloader::save_resource_files(const Download_info& download_info)
{
    const auto& files = download_info.file_info_;
    const auto& file_buffers = download_info.file_buffers_;

    boost::filesystem::path directory = download_info.full_target_directory_.string();
    if (!boost::filesystem::is_directory(directory))
    {
        // Create the directory if it doesn't exist yet
        boost::filesystem::create_directories(directory);
    }

    // Iterate through all files
    for (std::size_t index = 0; index != files.size() && index != file_buffers.size(); ++index)
    {
        const auto& file_info = files[index];
        const auto& file_buffer = file_buffers[index];

        const auto& file_name = file_info.file_name;

        boost::filesystem::ofstream out(directory / file_name.string(), std::ofstream::out | std::ofstream::binary);
        out.write(file_buffer.data(), file_buffer.size());
    }
}
void ts::client::Resource_downloader::handle_finished_message(const Message& message)
{
    auto finished = downloads::parse_finished_message(message);
    auto download_it = download_map_.find(finished.download_key);
    if (download_it != download_map_.end())
    {
        auto& download_info = download_it->second;
        auto main_file = download_info.target_directory_;
        main_file += "/";
        main_file += download_info.file_info_.front().file_name;

        if (download_info.resource_type == Resource_type::Track)
        {
            resource_store_->track_store().register_track(main_file);
        }

        else if (download_info.resource_type == Resource_type::Car)
        {
            resource_store_->car_store().register_car_file(main_file);
        }

        save_resource_files(download_info);

        download_map_.erase(download_it);
    }
}

void ts::client::Resource_downloader::request_track(const resources::Track_identifier& track_identifier)
{
    auto key = generate_key();

    // Add download to map
    auto& download_info = download_map_[key];

    // Specify target directory
    download_info.target_directory_ = "download";

    download_info.full_target_directory_ = resource_store_->track_store().root_directory_path();
    download_info.full_target_directory_ += "/";
    download_info.full_target_directory_ += download_info.target_directory_;

    download_info.resource_type = Resource_type::Track;

    message_buffer_.message = downloads::make_track_download_request(key, track_identifier);

    // And send the request
    message_center_->dispatch_message(message_buffer_);

    cup::Composite_message chat_message;
    chat_message.append("Trying to download track ", sf::Color(255, 150, 0));
    chat_message.append(track_identifier.track_name, sf::Color(255, 220, 50));
    chat_message.append("...", sf::Color(255, 150, 0));

    message_buffer_.message = cup::make_chatbox_output_message(chat_message);
    message_center_->handle_message(message_buffer_);
}

void ts::client::Resource_downloader::request_car(const resources::Car_identifier& car_identifier)
{
    auto key = generate_key();
    
    auto& download_info = download_map_[key];

    download_info.target_directory_ += "download";
    download_info.full_target_directory_ = resource_store_->car_store().root_directory();
    download_info.full_target_directory_ += "/";
    download_info.full_target_directory_ += download_info.target_directory_;    

    message_buffer_.message = downloads::make_car_download_request(key, car_identifier);
    message_center_->dispatch_message(message_buffer_);

    cup::Composite_message chat_message;
    chat_message.append("Trying to download car ", sf::Color(255, 150, 0));
    chat_message.append(car_identifier.car_name, sf::Color(255, 220, 50));
    chat_message.append("...", sf::Color(255, 150, 0));

    message_buffer_.message = cup::make_chatbox_output_message(chat_message);
    message_center_->handle_message(message_buffer_);
}

bool ts::client::Resource_downloader::is_downloading() const
{
    return !download_map_.empty();
}

std::pair<std::size_t, std::size_t> ts::client::Resource_downloader::download_progress() const
{
    std::pair<std::size_t, std::size_t> result = std::make_pair(0, 0);
    for (auto& download_info : download_map_)
    {
        result.first += download_info.second.bytes_downloaded_;
        result.second += download_info.second.total_size_;
    }

    return result;
}