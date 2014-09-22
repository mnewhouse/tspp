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

ts::game::Cup::Cup(Cup_type cup_type, resources::Resource_store* resource_store)
: cup_type_(cup_type),
  current_track_index_(0),
  resource_store_(resource_store),
  car_settings_(&resource_store->settings.car_settings),
  track_settings_(&resource_store->settings.track_settings),
  script_settings_(&resource_store->settings.script_settings),
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

ts::game::Cup_type ts::game::Cup::cup_type() const
{
    return cup_type_;
}

ts::game::Cup_state ts::game::Cup::cup_state() const
{
    return state_;
}

ts::resources::Car_mode ts::game::Cup::car_mode() const
{
    return car_settings_->car_mode();
}

const std::vector<ts::resources::Car_handle>& ts::game::Cup::car_list() const
{
    return car_settings_->selected_cars();
}

ts::game::Stage_data ts::game::Cup::make_stage_data() const
{
    Stage_data stage_data;
    stage_data.cars.resize(player_map_.size());
    stage_data.track_handle = current_track();    

    std::transform(player_map_.begin(), player_map_.end(), stage_data.cars.begin(), 
                   [](const std::pair<Player_id, Cup_player_data>& player)
    {
        return player.second.car_data;
    });


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

ts::game::Cup::Player_handle ts::game::Cup::add_local_player(const Player& player, controls::Slot control_slot)
{
    if (control_slot >= local_players_.size())
    {
        local_players_.resize(control_slot + 1);
    }

    if (auto old_player = local_players_[control_slot])
    {
        player_map_.erase(old_player->player_id);
    }

    auto unique_id = allocate_player_id();

    Cup_player_data player_data;
    player_data.player_id = unique_id;
    player_data.player = player;
    player_data.car_data.control_slot = control_slot;
    player_data.car_data.start_pos = player_map_.size();
    player_data.car_data.color = player.color;

    auto it = player_map_.emplace(unique_id, player_data).first;

    Player_handle result(&it->second);
    local_players_[control_slot] = result;

    return result;
}

ts::game::Player_id ts::game::Cup::allocate_player_id() const
{
    Player_id last = 0;
    for (const auto& player : player_map_)
    {
        if (player.first - last > 1) break;

        last = player.first;
    }

    return last + 1;
}

const std::vector<ts::game::Cup::Player_handle>& ts::game::Cup::local_players() const
{
    return local_players_;
}

std::size_t ts::game::Cup::player_count() const
{
    return player_map_.size();
}

void ts::game::Cup::set_player_car(Player_handle player, resources::Car_handle car)
{
    auto player_data = player_map_.find(player->player_id);
    if (player_data != player_map_.end())
    {
        player_data->second.car_data.car = car;
    }
}

void ts::game::Cup::add_track(resources::Track_handle track_handle)
{
    resource_store_->settings.track_settings.selected_tracks.push_back(track_handle);
}

const std::vector<ts::resources::Track_handle>& ts::game::Cup::track_list() const
{
    auto& track_settings = resource_store_->settings.track_settings;
    return track_settings.selected_tracks;
}

void ts::game::Cup::advance()
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

void ts::game::Cup::signal_ready()
{
    if (cup_type() == Cup_type::Local)
    {
        switch (cup_state())
        {
        case Cup_state::Car_selection:
        case Cup_state::Initializing:
            advance();
            break;
        default:        
            break;
        }
    }
}

void ts::game::Cup::add_cup_listener(Cup_listener* cup_listener)
{
    cup_listeners_.push_back(cup_listener);
}

void ts::game::Cup::remove_cup_listener(Cup_listener* cup_listener)
{
    cup_listeners_.erase(std::remove(cup_listeners_.begin(), cup_listeners_.end(), cup_listener), cup_listeners_.end());
}

void ts::game::Cup::restart()
{    
    for (auto listener : cup_listeners_)
    {
        listener->on_restart();
    }

    current_track_index_ = 0;
}

void ts::game::Cup::change_state(Cup_state new_state)
{
    for (auto listener : cup_listeners_)
    {
        listener->on_state_change(state_, new_state);
    }

    state_ = new_state;
}

void ts::game::Cup::start_cup()
{
    if (track_list().empty())
    {
        change_state(Cup_state::End);
    }

    else
    {
        change_state(Cup_state::Cup);
    }
}

void ts::game::Cup::launch_action()
{
    change_state(Cup_state::Action);
}

void ts::game::Cup::stop_action()
{
    ++current_track_index_;

    if (current_track_index_ >= track_list().size())
    {
        change_state(Cup_state::End);
    }

    else
    {
        change_state(Cup_state::Cup);
    }
}

void ts::game::Cup::initialize_action()
{
    change_state(Cup_state::Initializing);
}

void ts::game::Cup::preinitialize_action()
{
    assign_start_positions();

    if (car_mode() == resources::Car_mode::Free && car_list().size() > 1)
    {
        change_state(Cup_state::Car_selection);
    }

    else
    {
        for (auto& player : player_map_)
        {
            player.second.car_data.car = car_list().front();
        }

        initialize_action();
    }
}

void ts::game::Cup::assign_start_positions()
{
    int start_pos = 0;
    for (auto& player : player_map_)
    {
        player.second.car_data.start_pos = start_pos++;
    }
}

ts::resources::Track_handle ts::game::Cup::current_track() const
{
    const auto& track_list = resource_store_->settings.track_settings.selected_tracks;

    if (current_track_index_ < track_list.size()) return track_list[current_track_index_];

    return ts::resources::Track_handle();
}

ts::game::Cup::Player_handle::Player_handle()
: player_data_(nullptr)
{
}

ts::game::Cup::Player_handle::Player_handle(const ts::game::Cup_player_data* player_data)
: player_data_(player_data)
{
}

ts::game::Cup::Player_handle::operator bool() const
{
    return player_data_ != nullptr;
}

const ts::game::Cup_player_data* ts::game::Cup::Player_handle::operator->() const
{
    return player_data_;
}

const ts::game::Cup_player_data& ts::game::Cup::Player_handle::operator*() const
{
    return *player_data_;
}

bool ts::game::Cup::Player_handle::operator==(Player_handle other) const
{
    return player_data_ == other.player_data_;
}

bool ts::game::Cup::Player_handle::operator!=(Player_handle other) const
{
    return player_data_ != other.player_data_;
}