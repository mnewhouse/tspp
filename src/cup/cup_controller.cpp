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
#include "cup_controller.hpp"
#include "cup.hpp"
#include "cup_config.hpp"

#include "resources/resource_store.hpp"
#include "resources/car_handle.hpp"
#include "resources/car_store.hpp"

struct ts::cup::Cup_controller::Impl
{
    Impl(resources::Resource_store* resource_store);

    void start_cup();
    void preinitialize_action();
    void initiate_car_selection();
    void wait_for_initialization();
    void initialize_action();
    void launch_action();
    void end_action();

    Cup cup_;
    Cup_config cup_config_;

    const resources::Resource_store* resource_store_;
    std::vector<resources::Car_handle> selected_cars_;
    std::unordered_map<Player_handle, resources::Car_handle> car_mapping_;
    std::vector<Stage_player> stage_players_;
};

ts::cup::Cup_controller::Impl::Impl(resources::Resource_store* resource_store)
: cup_(Locality::Local),
  cup_config_(resource_store),
  resource_store_(resource_store)
{
}

ts::cup::Cup_controller::Cup_controller(resources::Resource_store* resource_store)
: impl_(std::make_unique<Impl>(resource_store))
{
}

ts::cup::Cup_controller::~Cup_controller()
{
}

const ts::cup::Cup* ts::cup::Cup_controller::cup() const
{
    return &impl_->cup_;
}

ts::cup::Cup_state ts::cup::Cup_controller::cup_state() const
{
    return impl_->cup_.cup_state();
}

std::size_t ts::cup::Cup_controller::cup_progress() const
{
    return impl_->cup_.cup_progress();
}

std::size_t ts::cup::Cup_controller::stage_count() const
{
    return track_list().size();
}

ts::resources::Car_mode ts::cup::Cup_controller::car_mode() const
{
    return car_settings().car_mode();
}

const std::vector<ts::resources::Car_identifier>& ts::cup::Cup_controller::car_list() const
{
    return car_settings().selected_cars();
}

const ts::resources::Car_settings& ts::cup::Cup_controller::car_settings() const
{
    return impl_->cup_config_.car_settings();
}

const std::vector<ts::resources::Track_handle>& ts::cup::Cup_controller::track_list() const
{
    return track_settings().selected_tracks();
}

const ts::resources::Track_settings& ts::cup::Cup_controller::track_settings() const
{
    return impl_->cup_config_.track_settings();
}

void ts::cup::Cup_controller::skip_stage()
{
    const auto& tracks = track_list();
    auto progress = cup_progress();
    if (progress < tracks.size())
    {
        ++progress;
        impl_->cup_.set_cup_progress(progress);

        if (progress == tracks.size())
        {
            impl_->cup_.set_cup_state(Cup_state::End);
        }

        else
        {
            impl_->cup_.set_cup_state(Cup_state::Cup);
        }
    }
}

ts::resources::Track_handle ts::cup::Cup_controller::current_track() const
{
    const auto& tracks = track_list();
    const auto progress = cup_progress();

    if (progress >= tracks.size())
    {
        return resources::Track_handle();
    }

    return tracks[progress];
}

ts::cup::Player_handle ts::cup::Cup_controller::add_player(const Player& player, Player_id player_id)
{
    return impl_->cup_.add_player(player, player_id);
}

ts::cup::Player_handle ts::cup::Cup_controller::add_player(const Player& player)
{
    return impl_->cup_.add_player(player);
}

void ts::cup::Cup_controller::set_player_car(const Player_handle& player, const resources::Car_identifier& car_identifier)
{
    const auto& car_store = impl_->resource_store_->car_store();
    if (auto car_handle = car_store.get_car_by_name(car_identifier.car_name))
    {
        impl_->car_mapping_[player] = car_handle;
    }    
}

ts::resources::Car_handle ts::cup::Cup_controller::player_car(const Player_handle& player) const
{
    const auto& car_mapping = impl_->car_mapping_;
    auto it = car_mapping.find(player);
    if (it == car_mapping.end())
    {
        return resources::Car_handle();
    }

    return it->second;
}

void ts::cup::Cup_controller::initialize_action(const Stage_data& stage_data)
{
    impl_->cup_.set_cup_state(Cup_state::Initializing);

    impl_->cup_.initialize_action(stage_data);
}

const std::vector<ts::cup::Stage_player>& ts::cup::Cup_controller::stage_players() const
{
    return impl_->stage_players_;
}

void ts::cup::Cup_controller::add_cup_listener(Cup_listener* listener)
{
    impl_->cup_.add_cup_listener(listener);
}

void ts::cup::Cup_controller::remove_cup_listener(Cup_listener* listener)
{
    impl_->cup_.remove_cup_listener(listener);
}

void ts::cup::Cup_controller::advance()
{
    switch (cup_state())
    {
    case Cup_state::Registering:
        impl_->start_cup();
        break;

    case Cup_state::Cup:
        impl_->preinitialize_action();
        break;

    case Cup_state::Car_selection:
        impl_->wait_for_initialization();
        break;

    case Cup_state::Initializing:
        // The only way to get to this state is by calling initialize_action()
        // with a stage data object.

        impl_->launch_action();
        break;

    case Cup_state::Action:
        impl_->end_action();
        break;
    }
}

void ts::cup::Cup_controller::Impl::start_cup()
{
    const auto track_count = cup_config_.track_settings().selected_tracks().size();

    if (track_count == 0)
    {
        cup_.set_cup_state(Cup_state::End);
    }

    else
    {
        cup_.set_cup_state(Cup_state::Cup);
    }

    cup_.set_cup_progress(0);
}

void ts::cup::Cup_controller::Impl::preinitialize_action()
{
    const auto& car_settings = cup_config_.car_settings();
    const auto& car_store = resource_store_->car_store();
    const auto car_mode = car_settings.car_mode();


    // Store selected cars away, so that it doesn't bug out when 
    // cars are added or removed during car selection.
    selected_cars_.clear();
    for (const auto& car_identifier : car_settings.selected_cars())
    {
        if (auto car_handle = car_store.get_car_by_name(car_identifier.car_name))
        {
            selected_cars_.push_back(car_handle);
        }
    }

    stage_players_.clear();

    // Do the same for the cup's players.
    std::uint32_t start_pos = 0;
    for (auto player : cup_.player_list())
    {
        stage_players_.emplace_back();
        auto& stage_player = stage_players_.back();
        stage_player.player = player;
        stage_player.start_pos = start_pos++;
    }

    if (car_mode == resources::Car_mode::Free && selected_cars_.size() >= 2)
    {
        initiate_car_selection();
    }

    else
    {
        // We can skip car selection, because it's not needed.
        wait_for_initialization();
    }
}

void ts::cup::Cup_controller::Impl::initiate_car_selection()
{
    cup_.set_cup_state(Cup_state::Car_selection);
}

void ts::cup::Cup_controller::Impl::wait_for_initialization()
{
    if (cup_.cup_state() == Cup_state::Car_selection)
    {
        // Make car selection definitive
        for (auto& stage_player : stage_players_)
        {
            auto it = car_mapping_.find(stage_player.player);
            if (it == car_mapping_.end() || !it->second)
            {
                // Player chose no car. Just give them the first available car.
                // If the car list is empty too, we have to hope that a script or something else
                // sets the car for this player.
                stage_player.car = (selected_cars_.empty() ? resources::Car_handle() : selected_cars_.front());
            }

            else
            {
                // They have a car. Proceed.
                stage_player.car = it->second;
            }
        }
    }

    else
    {
        // No car selection, but we still have to give them their car.
        for (auto& stage_player : stage_players_)
        {
            stage_player.car = (selected_cars_.empty() ? resources::Car_handle() : selected_cars_.front());
        }
    }

    // TODO: custom start order.

    cup_.set_cup_state(Cup_state::Awaiting_initialization);
}

void ts::cup::Cup_controller::Impl::launch_action()
{
    cup_.set_cup_state(Cup_state::Action);
}

void ts::cup::Cup_controller::Impl::end_action()
{
    const auto track_count = cup_config_.track_settings().selected_tracks().size();
    const auto cup_progress = cup_.cup_progress() + 1;
    cup_.set_cup_progress(cup_progress);

    if (cup_progress >= track_count)
    {
        cup_.set_cup_state(Cup_state::End);
    }

    else
    {
        // And the sequence continues
        cup_.set_cup_state(Cup_state::Cup);
    }
}
