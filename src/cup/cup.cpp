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
#include "cup.hpp"
#include "player.hpp"
#include "cup_listener.hpp"

#include "resources/resource_store.hpp"

ts::cup::Cup::Cup(Cup_type cup_type, resources::Resource_store* resource_store)
: cup_type_(cup_type),
  cup_progress_(0),
  resource_store_(resource_store),
  car_settings_(&resource_store->settings.car_settings),
  track_settings_(&resource_store->settings.track_settings),
  script_settings_(&resource_store->settings.script_settings),
  cup_settings_(&resource_store->settings.cup_settings),
  state_(Cup_state::Registering)
{
    const auto& car_store = resource_store->cars;
    const auto& car_settings = resource_store->settings.car_settings;


    for (const auto& script_name : script_settings_->loaded_scripts)
    {
        auto script_resource = resource_store->scripts.get_script_by_name(script_name);
        if (script_resource)
        {
            loaded_scripts_.push_back(script_resource);
        }
    }    
}

ts::cup::Cup_type ts::cup::Cup::cup_type() const
{
    return cup_type_;
}

ts::cup::Cup_state ts::cup::Cup::cup_state() const
{
    return state_;
}

ts::resources::Car_mode ts::cup::Cup::car_mode() const
{
    return car_settings_->car_mode();
}

const std::vector<ts::resources::Car_handle>& ts::cup::Cup::car_list() const
{
    return car_settings_->selected_cars();
}

ts::cup::Player_handle ts::cup::Cup::add_player(const Player& player, controls::Slot control_slot)
{
    auto unique_id = allocate_player_id();

    Cup_player_data cup_player_data;
    cup_player_data.player_id = unique_id;
    cup_player_data.nickname = player.nickname;
    cup_player_data.color = player.color;
    cup_player_data.id = player.id;

    cup_player_data.car_data.color = player.color;
    cup_player_data.car_data.control_slot = control_slot;
    cup_player_data.car_data.start_pos = player_map_.size();
    cup_player_data.car_data.car = (car_list().empty() ? resources::Car_handle() : car_list().front());

    auto it = player_map_.emplace(unique_id, cup_player_data).first;

    Player_handle player_handle(&it->second);
    if (control_slot != controls::invalid_slot)
    {
        local_players_.push_back(player_handle);
    }

    return player_handle;
}

ts::cup::Player_id ts::cup::Cup::allocate_player_id() const
{
    Player_id last = 0;
    for (const auto& player : player_map_)
    {
        if (player.first - last > 1) break;

        last = player.first;
    }

    return last + 1;
}

const std::vector<ts::cup::Player_handle>& ts::cup::Cup::local_players() const
{
    return local_players_;
}


std::size_t ts::cup::Cup::player_count() const
{
    return player_map_.size();
}

std::size_t ts::cup::Cup::max_players() const
{
    return cup_settings_->max_players;
}

void ts::cup::Cup::add_track(resources::Track_handle track_handle)
{
    resource_store_->settings.track_settings.selected_tracks.push_back(track_handle);
}

const std::vector<ts::resources::Track_handle>& ts::cup::Cup::track_list() const
{
    auto& track_settings = resource_store_->settings.track_settings;
    return track_settings.selected_tracks;
}

void ts::cup::Cup::add_cup_listener(Cup_listener* cup_listener)
{
    cup_listeners_.push_back(cup_listener);
}

void ts::cup::Cup::remove_cup_listener(Cup_listener* cup_listener)
{
    cup_listeners_.erase(std::remove(cup_listeners_.begin(), cup_listeners_.end(), cup_listener), cup_listeners_.end());
}

void ts::cup::Cup::end()
{
    for (auto listener : cup_listeners_)
    {
        listener->on_end();
    }
}

void ts::cup::Cup::restart()
{    
    for (auto listener : cup_listeners_)
    {
        listener->on_restart();
    }

    cup_progress_ = 0;
}

void ts::cup::Cup::set_cup_state(Cup_state new_state)
{
    for (auto listener : cup_listeners_)
    {
        listener->on_state_change(state_, new_state);
    }

    state_ = new_state;
}

void ts::cup::Cup::set_player_car(Player_handle player, resources::Car_handle car_handle)
{
    auto it = player_map_.find(player->player_id);
    if (it != player_map_.end())
    {
        it->second.car_data.car = car_handle;
    }
}


ts::cup::Stage_data ts::cup::Cup::make_stage_data() const
{
    Stage_data stage_data;
    stage_data.track_handle = current_track();

    for (auto& player_data : player_map_)
    {
        stage_data.cars.push_back(player_data.second.car_data);
    }

    const auto& car_settings = resource_store_->settings.car_settings;
    if (car_settings.car_mode() == resources::Car_mode::Fixed && !car_list().empty())
    {
        for (auto& player_data : stage_data.cars)
        {
            player_data.car = car_list().front();
        }
    }

    stage_data.loaded_scripts = loaded_scripts_;
    return stage_data;
}


void ts::cup::Cup::advance()
{
    switch (state_)
    {
    case Cup_state::Registering:
        start_cup();
        break;
    case Cup_state::Cup:
        preinitialize_action();
        break;
    case Cup_state::Car_selection:
        initialize_action();
        break;
    case Cup_state::Initializing:
        launch_action();
        break;
    case Cup_state::Action:
        stop_action();
        break;
    case Cup_state::End:
        restart();
        break;
    }
}

void ts::cup::Cup::start_cup()
{
    if (track_list().empty())
    {
        set_cup_state(Cup_state::End);
    }

    else
    {
        set_cup_state(Cup_state::Cup);
    }
}

void ts::cup::Cup::launch_action()
{
    set_cup_state(Cup_state::Action);
}

void ts::cup::Cup::stop_action()
{
    ++cup_progress_;

    if (cup_progress_ >= track_list().size())
    {
        set_cup_state(Cup_state::End);
    }

    else
    {
        set_cup_state(Cup_state::Cup);
    }
}

void ts::cup::Cup::initialize_action()
{
    set_cup_state(Cup_state::Initializing);
}

void ts::cup::Cup::preinitialize_action()
{
    if (car_mode() == resources::Car_mode::Free && car_list().size() > 1)
    {
        set_cup_state(Cup_state::Car_selection);
    }

    else
    {
        initialize_action();
    }
}

ts::resources::Track_handle ts::cup::Cup::current_track() const
{
    const auto& track_list = resource_store_->settings.track_settings.selected_tracks;

    if (cup_progress_ < track_list.size()) return track_list[cup_progress_];

    return ts::resources::Track_handle();
}
