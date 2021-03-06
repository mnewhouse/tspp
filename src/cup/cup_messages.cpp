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
#include "stage_data.hpp"

#include "resources/track_store.hpp"
#include "resources/car_store.hpp"

#include "resources/settings/car_settings.hpp"
#include "resources/settings/track_settings.hpp"
#include "resources/settings/player_settings.hpp"

#include "messages/message_reader.hpp"

namespace ts
{
    namespace cup
    {
        using messages::Message_reader;

        Message_reader& operator>>(Message_reader&, sf::Color& color);
        Message_reader& operator>>(Message_reader&, resources::Color_base& color_base);
        Message_reader& operator>>(Message_reader&, resources::Color_extra& color_extra);
        Message_reader& operator>>(Message_reader&, resources::Player_color& player_color);

        Message& operator<<(Message&, sf::Color color);
        Message& operator<<(Message&, const resources::Color_base& color_base);
        Message& operator<<(Message&, const resources::Color_extra& color_extra);
        Message& operator<<(Message&, const resources::Player_color& player_color);

        static const std::uint64_t registration_key_mask = 0x6CCE0AA034E678A7;
    }
}

ts::cup::Message& ts::cup::operator<<(Message& message, sf::Color color)
{
    return message << color.r << color.g << color.b << color.a;
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
    return reader >> color.r >> color.g >> color.b >> color.a;
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

ts::cup::Message ts::cup::make_registration_request_message(std::uint64_t registration_key, const resources::Player_settings& player_settings, 
                                                            const resources::Player_store& player_store)
{
    std::vector<std::pair<std::int16_t, resources::Player_handle>> players;

    std::int16_t slot = 0;
    for (auto player_id : player_settings.selected_players())
    {
        auto player_handle = player_store.get_player_by_id(player_id);
        if (player_handle)
        {
            players.push_back(std::make_pair(slot, player_handle));
        }

        ++slot;
    }

    Message message(Message_type::registration_request);
    message << registration_key;
    message << static_cast<std::uint32_t>(players.size());

    for (auto player_info : players)
    {
        auto slot = player_info.first;
        auto player_handle = player_info.second;
        
        message << slot;
        message << player_handle->id;
        message << player_handle->name;
        message << player_handle->color;
    }

    return message;
}

ts::cup::Registration_request ts::cup::parse_registration_request_message(const Message& message)
{
    Message_reader message_reader(message);
    Registration_request result;

    std::uint32_t player_count = 0;
    if (message_reader >> result.message_type >> result.registration_key >> player_count)
    {
        Player_definition p;
        std::int16_t slot = 0;

        for (std::uint32_t idx = 0; idx != player_count && message_reader >> slot >> p.id >> p.nickname >> p.color; ++idx)
        {
            p.control_slot = slot;
            result.players.push_back(p);
        }
    }

    return result;
}

ts::cup::Message ts::cup::make_registration_acknowledgement_message(std::uint64_t registration_key)
{
    registration_key ^= registration_key_mask;

    Message result(Message_type::registration_acknowledgement);
    result << registration_key;
    return result;
}

ts::cup::Registration_acknowledgement ts::cup::parse_registration_acknowledgement_message(const Message& message)
{
    Message_reader message_reader(message);
    Registration_acknowledgement result;

    if (message_reader >> result.message_type >> result.registration_key)
    {
        result.registration_key ^= registration_key_mask;
    }

    return result;
}

ts::cup::Message ts::cup::make_bad_request_message()
{
    return Message(Message_type::bad_request);
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

ts::cup::Registration_refusal ts::cup::parse_registration_refusal_message(const Message& message)
{
    Registration_refusal join_refusal;
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

ts::cup::Message ts::cup::make_cup_progress_message(std::size_t progress, const resources::Track_identifier& track)
{
    Message message(Message_type::cup_progress);
    message << progress;
    message << track.track_name;

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
    message_reader >> result.message_type >> result.cup_progress >> result.track_identifier.track_name;

    return result;
}

ts::cup::Message ts::cup::make_player_information_message(const std::vector<Player_definition>& local_players, const std::vector<Player_handle>& remote_players)
{
    Message message(Message_type::player_information);
    message << static_cast<std::uint32_t>(local_players.size() + remote_players.size());

    for (auto player : local_players)
    {
        message << static_cast<std::uint16_t>(player.handle);
        message << static_cast<std::int16_t>(player.control_slot);
        message << player.id;
        message << player.nickname;
        message << player.color;
    }

    for (auto player : remote_players)
    {
        message << static_cast<std::uint16_t>(player->handle);
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
            std::uint16_t handle;
            std::int16_t control_slot;
            std::uint64_t id;
            utf8_string nickname;
            resources::Player_color color;

            if (message_reader >> handle >> control_slot >> id >> nickname >> color)
            {
                Player_definition player;
                player.handle = handle;                
                player.control_slot = control_slot;
                player.color = color;
                player.id = id;
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
        message << static_cast<std::uint16_t>(entry.player_id);
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

ts::cup::Message ts::cup::make_car_information_message(const resources::Car_settings& car_settings)
{
    const auto& selected_cars = car_settings.selected_cars();

    Message result(Message_type::car_information);
    
    result << static_cast<std::uint16_t>(car_settings.car_mode());
    result << static_cast<std::uint32_t>(selected_cars.size());
    for (const auto& car_identifier : selected_cars)
    {
        result << car_identifier.car_name;
    }

    return result;
}

ts::cup::Car_information_message ts::cup::parse_car_information_message(const Message& message)
{
    Car_information_message result;
    Message_reader message_reader(message);

    std::uint32_t car_count = 0;
    std::uint16_t car_mode = 0;
    if (message_reader >> result.message_type >> car_mode >> car_count)
    {
        resources::Car_identifier car_identifier;
        for (std::uint32_t n = 0; n != car_count && message_reader >> car_identifier.car_name; ++n)
        {
            result.cars.push_back(car_identifier);
        }

        result.car_mode = static_cast<resources::Car_mode>(car_mode);
    }

    return result;
}

ts::cup::Message ts::cup::make_car_selection_initiation_message(const resources::Car_settings& car_settings)
{
    const auto& selected_cars = car_settings.selected_cars();
    Message message(Message_type::car_selection_initiation);

    message << static_cast<std::uint32_t>(selected_cars.size());
    for (const auto& car_identifier : selected_cars)
    {
        message << car_identifier.car_name;
    }

    return message;
}

ts::cup::Car_information_message ts::cup::parse_car_selection_initiation_message(const Message& message)
{    
    Car_information_message result;
    result.car_mode = resources::Car_mode::Free;
    Message_reader message_reader(message);

    std::uint32_t car_count = 0;
    if (message_reader >> result.message_type >> car_count)
    {
        resources::Car_identifier car_identifier;
        for (std::uint32_t n = 0; n != car_count && message_reader >> car_identifier.car_name; ++n)
        {
            result.cars.push_back(car_identifier);
        }
    }

    return result;
}

ts::cup::Message ts::cup::make_ready_signal_message()
{
    return Message(Message_type::ready_signal);
}

ts::cup::Message ts::cup::make_advance_request_message()
{
    return Message(Message_type::advance_request);
}

ts::cup::Message ts::cup::make_client_quit_message()
{
    return Message(Message_type::client_quit);
}

ts::cup::Message ts::cup::make_chat_message(const utf8_string& message)
{
    Message result(Message_type::chat_message);
    result << message;

    return result;
}

ts::cup::Chat_message_definition ts::cup::parse_chat_message(const Message& message)
{
    Message_reader message_reader(message);
    Chat_message_definition result;

    message_reader >> result.message_type >> result.message;

    return result;
}

ts::cup::Message ts::cup::make_chatbox_output_message(const Chat_message& message)
{
    Message result(Message_type::chatbox_output);

    const auto& components = message.components();
    result << static_cast<std::uint32_t>(components.size());

    for (const auto& component : components)
    {
        result << component.color;
        result << component.text;
    }

    return result;
}

ts::cup::Chatbox_output_message ts::cup::parse_chatbox_output_message(const Message& message)
{
    Message_reader message_reader(message);
    Chatbox_output_message result;

    std::uint32_t num_components = 0;
    if (message_reader >> result.message_type >> num_components)
    {
        Chat_message_component component;
        for (std::uint32_t n = 0; n != num_components && message_reader >> component.color >> component.text; ++n)
        {
            result.message.append(component);
        }
    }

    return result;
}

ts::cup::Message ts::cup::make_action_initialization_message(const Stage_data& stage_data)
{
    Message message(Message_type::action_initialization);

    std::vector<resources::Car_handle> car_models;
    for (const auto& car_info : stage_data.cars)
    {
        if (car_info.car_def.model)
        {
            if (std::find(car_models.begin(), car_models.end(), car_info.car_def.model) == car_models.end())
            {
                car_models.push_back(car_info.car_def.model);
            }
        }    
    }

    message << stage_data.track.name();

    // Script resources
    const auto& script_resources = stage_data.script_resources;
    message << static_cast<std::uint32_t>(script_resources.size());
    for (auto resource : script_resources)
    {
        message << resource->name();
        // TODO: Resource hash
    }

    // Cars
    message << static_cast<std::uint32_t>(car_models.size());
    for (const auto& car_models : car_models)
    {
        message << car_models->car_name;
        // TODO: Car hash
    }

    message << static_cast<std::uint32_t>(stage_data.cars.size());
    for (const auto& car_info : stage_data.cars)
    {
        const auto& car_def = car_info.car_def;

        auto model_it = std::find(car_models.begin(), car_models.end(), car_def.model);
        if (model_it != car_models.end())
        {
            std::uint16_t resource_index = std::find(script_resources.begin(), script_resources.end(), car_def.resource) - script_resources.begin();
            std::uint16_t model_index = model_it - car_models.begin();

            message << model_index;
            message << resource_index;
            message << static_cast<std::uint16_t>(car_info.start_pos);
            message << static_cast<std::uint16_t>(car_info.car_id);

            std::uint16_t controller_id = car_info.controller ? car_info.controller->handle : 0;
            message << static_cast<std::uint16_t>(controller_id);
            message << car_info.player.color;
        }

    }

    return message;
}

ts::cup::Action_initialization_message ts::cup::parse_action_initialization_message(const Message& message)
{
    Message_reader message_reader(message);
    Action_initialization_message result;

    std::vector<utf8_string> car_names;
    std::uint32_t resource_count = 0, car_model_count = 0;
    if (message_reader >> result.message_type >> result.track_identifier.track_name >> resource_count)
    {
        utf8_string resource_name;
        for (std::uint32_t n = 0; n != resource_count && message_reader >> resource_name; ++n)
        {
            result.resource_list.emplace_back();
            result.resource_list.back().resource_name = resource_name;
        }

        if (message_reader >> car_model_count)
        {
            std::cout << car_model_count << std::endl;

            utf8_string car_name;
            for (std::uint32_t n = 0; n != car_model_count && message_reader >> car_name; ++n)
            {
                std::cout << car_name << std::endl;
                car_names.push_back(car_name);
            }

            std::uint32_t car_count = 0;
            if (message_reader >> car_count)
            {
                Action_initialization_message::Car car_def;
                std::uint16_t car_index = 0, resource_index = 0;
                for (std::uint32_t n = 0; n != car_count && message_reader >> car_index >> resource_index >> car_def.start_pos >>
                    car_def.car_id >> car_def.player >> car_def.color; ++n)
                {
                    if (car_index >= car_names.size()) continue;

                    car_def.car_identifier.car_name = car_names[car_index];
                    car_def.resource_index = resource_index;
                    result.car_list.push_back(car_def);
                }
            }
        }
    }

    return result;
}

ts::cup::Message ts::cup::make_server_quit_message()
{
    return Message(Message_type::server_quit);
}

ts::cup::Message ts::cup::make_load_error_message(const utf8_string& error_string)
{
    Message result(Message_type::load_error);
    result << error_string;
    return result;
}

ts::cup::Load_error_message ts::cup::parse_load_error_message(const Message& message)
{
    Load_error_message result;
    Message_reader message_reader(message);

    message_reader >> result.message_type >> result.error_string;
    return result;
}