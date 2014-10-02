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
#include "cup_messages.hpp"
#include "network/message_reader.hpp"

namespace ts
{
    namespace cup
    {
        using network::Message_reader;

        Message_reader& operator>>(Message_reader&, sf::Color& color);
        Message_reader& operator>>(Message_reader&, resources::Color_base& color_base);
        Message_reader& operator>>(Message_reader&, resources::Color_extra& color_extra);
        Message_reader& operator>>(Message_reader&, resources::Player_color& player_color);

        Message& operator<<(Message&, sf::Color color);
        Message& operator<<(Message&, const resources::Color_base& color_base);
        Message& operator<<(Message&, const resources::Color_extra& color_extra);
        Message& operator<<(Message&, const resources::Player_color& player_color);
    }
}

ts::cup::Message& ts::cup::operator<<(Message& message, sf::Color color)
{
    return message << color.r << color.g << color.b;
}

ts::cup::Message& ts::cup::operator<<(Message& message, const resources::Color_base& color_base)
{
    message << static_cast<std::uint16_t>(color_base.type);
    message << color_base.primary_color << color_base.secondary_color;

    return message;
}

ts::cup::Message& ts::cup::operator<<(Message& message, const resources::Color_extra& color_extra)
{
    message << static_cast<std::uint16_t>(color_extra.type);
    
    if (color_extra.type != resources::Color_extra::None)
    {
        message << color_extra.primary_color << color_extra.secondary_color;
    }

    return message;
}

ts::cup::Message& ts::cup::operator<<(Message& message, const resources::Player_color& player_color)
{
    return message << player_color.base_color << player_color.primary_extra << player_color.secondary_extra;
}

ts::cup::Message_reader& ts::cup::operator>>(Message_reader& reader, sf::Color& color)
{
    return reader >> color.r >> color.g >> color.b;
}

ts::cup::Message_reader& ts::cup::operator>>(Message_reader& reader, resources::Color_base& color_base)
{
    std::uint16_t type;
    if (reader >> type)
    {
        color_base.type = static_cast<resources::Color_base::Type>(type);
        reader >> color_base.primary_color >> color_base.secondary_color;
    }

    return reader;
}

ts::cup::Message_reader& ts::cup::operator>>(Message_reader& reader, resources::Color_extra& color_extra)
{
    std::uint16_t type;
    if (reader >> type)
    {
        color_extra.type = static_cast<resources::Color_extra::Type>(type);
        if (color_extra.type != resources::Color_extra::None)
        {
            reader >> color_extra.primary_color >> color_extra.secondary_color;
        }
    }

    return reader;
}

ts::cup::Message_reader& ts::cup::operator>>(Message_reader& reader, resources::Player_color& player_color)
{
    return reader >> player_color.base_color >> player_color.primary_extra >> player_color.secondary_extra;
}

ts::cup::Message ts::cup::make_join_request_message(std::uint64_t join_key, const resources::Player_settings& player_settings, 
                                                              const resources::Player_store& player_store)
{
    std::vector<resources::Player_handle> players;
    for (auto player_id : player_settings.selected_players)
    {
        if (auto player_handle = player_store.get_player_by_id(player_id))
        {
            players.push_back(player_handle);
        }
    }

    Message message(Message_type::join_request);
    message << join_key;

    std::uint32_t player_count = players.size();
    message << player_count;
    for (auto player_handle : players)
    {
        message << player_handle->id;
        message << player_handle->name;
        message << player_handle->color;
    }

    return message;
}

ts::cup::Join_request ts::cup::parse_join_request_message(const Message& message)
{
    Message_reader message_reader(message);
    Join_request result;

    std::uint32_t player_count = 0;
    if (message_reader >> result.message_type >> result.registration_key >> player_count && 
        result.message_type == Message_type::join_request)
    {
        for (std::uint32_t idx = 0; idx != player_count; ++idx)
        {
            Player player;
            if (message_reader >> player.id >> player.nickname >> player.color)
            {
                result.players.push_back(player);
            }
        }
    }

    return result;
}

ts::cup::Message ts::cup::make_join_acknowledgement_message(std::uint64_t join_key, std::uint32_t client_key)
{
    join_key ^= client_key;
    join_key ^= static_cast<std::uint64_t>(client_key * client_key) << 32;

    Message result(Message_type::join_acknowledgement);
    result << join_key;
    result << client_key;
    return result;
}

ts::cup::Join_acknowledgement ts::cup::parse_join_acknowledgement_message(const Message& message)
{
    Message_reader message_reader(message);
    Join_acknowledgement result;

    if (message_reader >> result.message_type >> result.registration_key >> result.client_key)
    {
        result.registration_key ^= result.client_key;
        result.registration_key ^= static_cast<std::uint64_t>(result.client_key * result.client_key) << 32;
    }

    return result;
}

ts::cup::Message ts::cup::make_bad_request_message(std::uint64_t registration_key)
{
    Message message(Message_type::bad_request);

    registration_key ^= Message_type::bad_request;
    registration_key ^= static_cast<std::uint64_t>(~Message_type::bad_request) << 32;

    message << registration_key;
    return message;
}

ts::cup::Message ts::cup::make_too_many_players_message(std::uint64_t registration_key)
{
    Message message(Message_type::too_many_players);
    
    registration_key ^= Message_type::too_many_players;
    registration_key ^= static_cast<std::uint64_t>(~Message_type::too_many_players) << 32;

    message << registration_key;
    return message;
}

ts::cup::Message ts::cup::make_version_mismatch_message(std::uint64_t registration_key)
{
    Message message(Message_type::version_mismatch);
    
    registration_key ^= Message_type::version_mismatch;
    registration_key ^= static_cast<std::uint64_t>(~Message_type::version_mismatch) << 32;
    message << registration_key;
    return message;
}

ts::cup::Join_refusal ts::cup::parse_join_refusal_message(const Message& message)
{
    Join_refusal join_refusal;
    Message_reader message_reader(message);
    if (message_reader >> join_refusal.message_type >> join_refusal.registration_key)
    {
        join_refusal.registration_key ^= join_refusal.message_type;
        join_refusal.registration_key ^= static_cast<std::uint64_t>(~join_refusal.message_type) << 32;
    }

    return join_refusal;    
}