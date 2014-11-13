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
#include "download_messages.hpp"

#include "messages/message_reader.hpp"

namespace ts
{
    namespace downloads
    {
        using messages::Message_reader;

        static ts::downloads::Download_message parse_generic_download_message(const Message& message);
    }
}

ts::downloads::Message ts::downloads::make_track_download_request(std::uint32_t download_key, const resources::Track_identifier& track_definition)
{
    Message message(Message_type::track_download_request);
    message << download_key;
    
    for (auto v : track_definition.track_hash)
    {
        message << v;
    }

    message << track_definition.track_name;
    return message;
}

ts::downloads::Track_download_request ts::downloads::parse_track_download_request(const Message& message)
{    
    Message_reader message_reader(message);

    Track_download_request request;
    std::uint32_t message_type;

    if (message_reader >> message_type >> request.download_key)
    {
        for (auto& v : request.track_identifier.track_hash)
        {
            message_reader >> v;
        }

        message_reader >> request.track_identifier.track_name;
    }

    return request;
}

ts::downloads::Message ts::downloads::make_car_download_request(std::uint32_t download_key, const resources::Car_identifier& car_identifier)
{
    Message message(Message_type::car_download_request);
    message << download_key;
    message << car_identifier.car_name;
    return message;
}

ts::downloads::Car_download_request ts::downloads::parse_car_download_request(const Message& message)
{
    Message_reader message_reader(message);

    Car_download_request request;
    std::uint32_t message_type;
    message_reader >> message_type >> request.download_key >> request.car_identifier.car_name;

    return request;
}

ts::downloads::Message ts::downloads::make_file_info_message(std::uint32_t download_key, const std::vector<File_info>& file_info)
{
    Message message(Message_type::file_info);
    message << download_key;
    message << static_cast<std::uint32_t>(file_info.size());
    
    for (const auto& file : file_info)
    {
        message << static_cast<std::uint32_t>(file.file_size);
        message << file.file_name;
    }

    return message;
}

ts::downloads::File_list ts::downloads::parse_file_info_message(const Message& message)
{  
    Message_reader message_reader(message);
    File_list file_list;
    std::uint32_t message_type, file_count;
    if (message_reader >> message_type >> file_list.download_key >> file_count)
    {
        for (File_info file_info; file_count != 0 && message_reader >> file_info.file_size >> file_info.file_name; --file_count)
        {
            file_list.file_info.push_back(file_info);
        }
    }

    return file_list;
}

ts::downloads::Message ts::downloads::make_file_chunk_message(std::uint32_t download_key, const std::uint8_t* data, std::size_t data_size)
{
    Message message(Message_type::file_chunk);
    message << download_key;
    message << data_size;
    message.append(data, data_size);
    return message;
}

ts::downloads::File_chunk ts::downloads::parse_file_chunk_message(const Message& message)
{
    Message_reader message_reader(message);
    File_chunk file_chunk;
    std::uint32_t message_type, data_size;
    if (message_reader >> message_type >> file_chunk.download_key >> data_size)
    {
        auto& buffer = file_chunk.chunk;
        buffer.resize(data_size);

        data_size = message_reader.read(buffer.data(), data_size);
        buffer.resize(data_size);
    }

    return file_chunk;
}

ts::downloads::Download_message ts::downloads::parse_generic_download_message(const Message& message)
{
    Message_reader message_reader(message);
    Download_message result;
    std::uint32_t message_type;
    message_reader >> message_type >> result.download_key;
    return result;
}


ts::downloads::Message ts::downloads::make_eof_message(std::uint32_t download_key)
{
    Message message(Message_type::eof);
    message << download_key;
    return message;
}

ts::downloads::Download_message ts::downloads::parse_eof_message(const Message& message)
{
    return parse_generic_download_message(message);
}

ts::downloads::Message ts::downloads::make_finished_message(std::uint32_t download_key)
{
    Message message(Message_type::finished);
    message << download_key;
    return message;
}

ts::downloads::Download_message ts::downloads::parse_finished_message(const Message& message)
{
    return parse_generic_download_message(message);
}

ts::downloads::Message ts::downloads::make_pong_message(std::uint32_t download_key)
{
    Message message(Message_type::pong);
    message << download_key;
    return message;
}

ts::downloads::Download_message ts::downloads::parse_pong_message(const Message& message)
{
    return parse_generic_download_message(message);
}

ts::downloads::Message ts::downloads::make_download_refusal_message(std::uint32_t download_key)
{
    Message message(Message_type::download_refused);
    message << download_key;
    return message;
}

ts::downloads::Download_message ts::downloads::parse_download_refusal_message(const Message& message)
{
    return parse_generic_download_message(message);
}