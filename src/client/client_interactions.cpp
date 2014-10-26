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
#include "client_interactions.hpp"
#include "resource_downloader.hpp"

#include "cup/cup_messages.hpp"
#include "cup/stage_data.hpp"
#include "cup/cup.hpp"

#include "resources/resource_store.hpp"
#include "resources/track_store.hpp"
#include "resources/car_store.hpp"

#include "resources/settings/track_settings.hpp"
#include "resources/settings/car_settings.hpp"

class ts::client::Interaction_interface::Impl
    : public Message_listener
{
public:
    Impl(Message_center* message_center, cup::Cup* cup, resources::Resource_store* resource_store);

    void send_registration_request();

    const utf8_string& registration_error() const;
    Registration_status registration_status() const;

    bool is_downloading() const;
    std::pair<std::size_t, std::size_t> download_progress() const;

private:
    virtual void handle_message(const Server_message& message) override;

    void handle_registration_acknowledgement_message(const Message& message);
    void handle_player_information_message(const Message& message);
    void handle_track_information_message(const Message& message);
    void handle_car_information_message(const Message& message);
    void handle_cup_state_message(const Message& message);
    void handle_cup_progress_message(const Message& message);
    void handle_action_initialization_message(const Message& message);
    void handle_server_quit_message(const Message& message);

    void registration_error(utf8_string error_string);

    Message_center* message_center_;
    cup::Cup* cup_;
    resources::Resource_store* resource_store_;

    Registration_status registration_status_ = Registration_status::None;
    std::uint64_t registration_key_;
    utf8_string registration_error_;

    std::unique_ptr<cup::Action_initialization_message> queued_init_message_;

    Resource_downloader resource_downloader_;
};

ts::client::Interaction_interface::Impl::Impl(Message_center* message_center, cup::Cup* cup, resources::Resource_store* resource_store)
: Message_listener(message_center),
  message_center_(message_center),
  cup_(cup),
  resource_store_(resource_store),
  resource_downloader_(message_center, resource_store)
{
}

void ts::client::Interaction_interface::Impl::send_registration_request()
{
    if (registration_status_ != Registration_status::Registered &&
        registration_status_ != Registration_status::Registering)
    {
        std::mt19937_64 key_generator(std::random_device{}());
        registration_key_ = key_generator();
        registration_status_ = Registration_status::Registering;

        Server_message out;
        out.message = cup::make_registration_request_message(registration_key_, resource_store_->player_settings(), resource_store_->player_store());
        out.message_type = Message_type::Reliable;

        message_center_->dispatch_message(out);        
    }
}

void ts::client::Interaction_interface::Impl::handle_message(const Server_message& server_message)
{
    if (server_message.message_type == Message_type::Reliable)
    {
        using Msg = cup::Message_type;
        const auto& message = server_message.message;

        switch (message.type())
        {
        case Msg::registration_acknowledgement:
            handle_registration_acknowledgement_message(message);
            break;

        case Msg::player_information:
            handle_player_information_message(message);
            break;

        case Msg::track_information:
            handle_track_information_message(message);
            break;

        case Msg::car_information:
            handle_car_information_message(message);
            break;

        case Msg::cup_progress:
            handle_cup_progress_message(message);
            break;

        case Msg::cup_state:
            handle_cup_state_message(message);
            break;

        case Msg::action_initialization:
            handle_action_initialization_message(message);
            break;

        case Msg::server_quit:
            handle_server_quit_message(message);
            break;
        }
    }
}

void ts::client::Interaction_interface::Impl::handle_registration_acknowledgement_message(const Message& message)
{
    auto reg_acknowledgement = cup::parse_registration_acknowledgement_message(message);
    if (reg_acknowledgement.registration_key == registration_key_)
    {
        registration_status_ = Registration_status::Registered;
    }

    else
    {
        registration_error("bad server response");
    }
}

void ts::client::Interaction_interface::Impl::handle_player_information_message(const Message& message)
{
    auto player_information = cup::parse_player_information_message(message);

    cup_->clear_players();

    for (const auto& player : player_information.players)
    {
        cup_->add_player(player, player.handle);
    }
}

void ts::client::Interaction_interface::Impl::handle_track_information_message(const Message& message)
{
    auto track_information = cup::parse_track_information_message(message);
    
    cup_->clear_tracks();

    const auto& track_store = resource_store_->track_store();
    for (const auto& track_name : track_information.track_names)
    {
        auto track_handle = track_store.get_track_by_name(track_name);
        cup_->add_track(track_handle);
    }
}

void ts::client::Interaction_interface::Impl::handle_car_information_message(const Message& message)
{
    auto car_information = cup::parse_car_information_message(message);

    const auto& car_store = resource_store_->car_store();

    resources::Car_settings car_settings;
    car_settings.set_car_mode(car_information.car_mode);

    for (const auto& car_identifier : car_information.cars)
    {
        auto car_handle = car_store.get_car_by_name(car_identifier.car_name);
        if (!car_handle)
        {
            resource_downloader_.request_car(car_identifier);
        }

        car_settings.select_car(car_handle);
    }

    cup_->load_car_settings(car_settings);
}


void ts::client::Interaction_interface::Impl::handle_cup_state_message(const Message& message)
{
    auto state_info = cup::parse_cup_state_message(message);
    cup_->set_cup_state(state_info.cup_state);
}

void ts::client::Interaction_interface::Impl::handle_cup_progress_message(const Message& message)
{
    auto progress_info = cup::parse_cup_progress_message(message);
    cup_->set_cup_progress(progress_info.cup_progress);

    const auto& track_name = progress_info.track_identifier.track_name;
    auto track_handle = resource_store_->track_store().get_track_by_name(track_name);
    if (!track_handle)
    {
        resource_downloader_.request_track(progress_info.track_identifier);
    }
}

void ts::client::Interaction_interface::Impl::handle_action_initialization_message(const Message& message)
{
    auto action_info = cup::parse_action_initialization_message(message);

    const auto& track_store = resource_store_->track_store();
    const auto& car_store = resource_store_->car_store();

    cup::Stage_data stage_data;
    stage_data.track = track_store.get_track_by_name(action_info.track_name);
    
    for (const auto& car_info : action_info.car_list)
    {
        // TODO: Handle resource downloading
        // TODO: Error out if car not found
        if (auto car_def = car_store.get_car_by_name(car_info.car_name))
        {
            cup::Car_data car_data;
            car_data.car_def = car_def;
            car_data.car_id = car_info.car_id;
            car_data.start_pos = car_info.start_pos;
            car_data.player.color = car_info.color;
            car_data.controller = cup_->get_player_by_id(car_info.player);

            stage_data.cars.push_back(car_data);
        }
    }
    
    cup_->initialize_action(stage_data);
}

void ts::client::Interaction_interface::Impl::handle_server_quit_message(const Message& message)
{
    cup::Composite_message displayed_message;
    displayed_message.append("Server quitting.", sf::Color(200, 0, 0));
    
    Server_message out_message;
    out_message.message_type = Message_type::Reliable;
    out_message.message = cup::make_chatbox_output_message(displayed_message);

    message_center_->handle_message(out_message);
}

void ts::client::Interaction_interface::Impl::registration_error(utf8_string error_string)
{
    registration_error_ = std::move(error_string);
}

const ts::utf8_string& ts::client::Interaction_interface::Impl::registration_error() const
{
    return registration_error_;
}

ts::client::Registration_status ts::client::Interaction_interface::Impl::registration_status() const
{
    return registration_status_;
}

bool ts::client::Interaction_interface::Impl::is_downloading() const
{
    return resource_downloader_.is_downloading();
}

std::pair<std::size_t, std::size_t> ts::client::Interaction_interface::Impl::download_progress() const
{
    return resource_downloader_.download_progress();
}

ts::client::Interaction_interface::Interaction_interface(Message_center* message_center, cup::Cup* cup, resources::Resource_store* resource_store)
: impl_(std::make_unique<Impl>(message_center, cup, resource_store))
{
}

ts::client::Interaction_interface::~Interaction_interface()
{
}

ts::client::Registration_status ts::client::Interaction_interface::registration_status() const
{
    return impl_->registration_status();
}

const ts::utf8_string& ts::client::Interaction_interface::registration_error() const
{
    return impl_->registration_error();
}

void ts::client::Interaction_interface::send_registration_request()
{
    impl_->send_registration_request();
}

bool ts::client::Interaction_interface::is_downloading() const
{
    return impl_->is_downloading();
}

std::pair<std::size_t, std::size_t> ts::client::Interaction_interface::download_progress() const
{
    return impl_->download_progress();
}