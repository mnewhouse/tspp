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

#include "resources/track_store.hpp"
#include "resources/car_store.hpp"

#include "resources/settings/car_settings.hpp"
#include "resources/settings/track_settings.hpp"
#include "resources/settings/player_settings.hpp"

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
    std::uint32_t player_count = 0;

    for (auto player_id : player_settings.selected_players())
    {
        auto player_handle = player_store.get_player_by_id(player_id);
        if (player_handle)
        {
            player_count = 0;
        }

        players.push_back(player_handle);
    }

    Message message(Message_type::join_request);
    message << join_key;
    message << player_count;

    std::int16_t slot = 0;
    for (auto player_handle : players)
    {
        if (player_handle)
        {
            message << slot;
            message << player_handle->id;
            message << player_handle->name;
            message << player_handle->color;
        }

        ++slot;
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
            if (message_reader >> player.control_slot >> player.id >> player.nickname >> player.color)
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

ts::cup::Message ts::cup::make_cup_state_message(Cup_state cup_state)
{
    Message message(Message_type::cup_state);
    message << static_cast<std::uint32_t>(cup_state);

    return message;
}

ts::cup::Message ts::cup::make_cup_progress_message(std::size_t progress, const resources::Track_handle& track)
{
    Message message(Message_type::cup_progress);
    message << progress;

    return message;
}

ts::cup::Cup_state_message ts::cup::parse_cup_state_message(const Message& message)
{
    Message_reader message_reader(message);

    Cup_state_message result;

    std::uint32_t cup_state = 0;
    if (message_reader >> result.message_type >> cup_state)
    {
        result.cup_state = static_cast<Cup_state>(cup_state);
    }

    return result;
}

ts::cup::Cup_progress_message ts::cup::parse_cup_progress_message(const Message& message)
{
    Message_reader message_reader(message);

    Cup_progress_message result;
    message_reader >> result.message_type >> result.cup_progress;

    return result;
}

ts::cup::Message ts::cup::make_player_information_message(const std::vector<Player_handle>& local_players, const std::vector<Player_handle>& remote_players)
{
    Message message(Message_type::player_information);
    message << static_cast<std::uint32_t>(local_players.size() + remote_players.size());
    for (auto player : local_players)
    {
        message << player->handle;
        message << static_cast<std::int16_t>(player->control_slot);
        message << player->id;
        message << player->nickname;
        message << player->color;
    }

    for (auto player : remote_players)
    {
        message << player->handle;
        message << static_cast<std::int16_t>(controls::invalid_slot);
        message << player->id;
        message << player->nickname;
        message << player->color;
    }    

    return message;
}

ts::cup::Player_information_message ts::cup::parse_player_information_message(const Message& message)
{
    Message_reader message_reader(message);
    Player_information_message result;

    std::uint32_t player_count = 0;
    if (message_reader >> result.message_type >> player_count)
    {
        for (std::uint32_t idx = 0; idx != player_count; ++idx)
        {
            Player_id handle;
            std::int16_t control_slot;
            utf8_string nickname;
            resources::Player_color color;

            if (message_reader >> handle >> control_slot >> nickname >> color)
            {
                Player_information_message::Player_definition player;
                player.handle = handle;
                player.control_slot = control_slot;
                player.color = color;
                player.nickname = std::move(nickname);

                result.players.push_back(player);
            }            
        }
    }

    return result;
}

ts::cup::Message ts::cup::make_car_selection_message(const std::vector<Car_selection>& car_selection)
{
    Message message(Message_type::car_selection);

    message << static_cast<std::uint32_t>(car_selection.size());
    for (const auto& entry : car_selection)
    {
        message << entry.player_handle->handle;
        message << static_cast<std::uint32_t>(entry.car_index);
    }

    return message;
}

ts::cup::Car_selection_message ts::cup::parse_car_selection_message(const Message& message)
{
    Message_reader message_reader(message);
    Car_selection_message result;

    std::uint32_t player_count = 0;
    if (message_reader >> result.message_type >> player_count)
    {
        Car_selection_message::Entry entry;
        for (std::uint32_t n = 0; n != player_count && message_reader >> entry.player_handle >> entry.car_id; ++n)
        {
            result.car_selection.push_back(entry);
        }
    }

    return result;
}

ts::cup::Message ts::cup::make_track_information_message(const resources::Track_settings& track_settings, const resources::Track_store& track_store)
{
    const auto& selected_tracks = track_settings.selected_tracks();

    Message result(Message_type::track_information);

    result << static_cast<std::uint32_t>(selected_tracks.size());
    for (auto& track_handle : selected_tracks)
    {
        result << track_handle.name();
    }

    return result;
}

ts::cup::Track_information_message ts::cup::parse_track_information_message(const Message& message)
{
    Track_information_message result;
    Message_reader message_reader(message);

    std::uint32_t track_count = 0;
    if (message_reader >> result.message_type >> track_count)
    {
        utf8_string track_name;
        for (std::uint32_t n = 0; n != track_count && message_reader >> track_name; ++n)
        {
            result.track_names.push_back(track_name);
        }
    }

    return result;
}

ts::cup::Message ts::cup::make_car_information_message(const resources::Car_settings& car_settings, const resources::Car_store& car_store)
{
    const auto& selected_cars = car_settings.selected_cars();

    Message result(Message_type::car_information);
    
    result << static_cast<std::uint16_t>(car_settings.car_mode());
    result << static_cast<std::uint32_t>(selected_cars.size());
    for (auto& car_handle : selected_cars)
    {
        result << car_handle.car_name();
    }

    return result;
}

ts::cup::Car_information_message ts::cup::parse_car_information_message(const Message& message)
{
    Car_information_message result;
    Message_reader message_reader(message);

    std::uint32_t track_count = 0;
    std::uint16_t car_mode = 0;
    if (message_reader >> result.message_type >> car_mode >> track_count)
    {
        utf8_string car_name;
        for (std::uint32_t n = 0; n != track_count && message_reader >> car_name; ++n)
        {
            result.car_names.push_back(car_name);
        }

        result.car_mode = static_cast<resources::Car_mode>(car_mode);
    }

    return result;
}

ts::cup::Message ts::cup::make_ready_signal_message()
{
    return Message(Message_type::ready_signal);
}