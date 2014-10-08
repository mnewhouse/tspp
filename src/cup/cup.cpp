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

ts::cup::Cup::Cup(Cup_type cup_type)
: cup_type_(cup_type),
  cup_progress_(0),
  state_(Cup_state::Registering)
{
}

ts::cup::Cup::~Cup()
{
}

ts::cup::Cup_type ts::cup::Cup::cup_type() const
{
    return cup_type_;
}

ts::cup::Cup_state ts::cup::Cup::cup_state() const
{
    return state_;
}

std::size_t ts::cup::Cup::cup_progress() const
{
    return cup_progress_;
}

void ts::cup::Cup::select_car(resources::Car_handle car_handle)
{
    car_settings_.select_car(car_handle);
}

void ts::cup::Cup::set_car_mode(resources::Car_mode car_mode)
{
    car_settings_.set_car_mode(car_mode);
}

ts::resources::Car_mode ts::cup::Cup::car_mode() const
{
    return car_settings_.car_mode();
}

const std::vector<ts::resources::Car_handle>& ts::cup::Cup::car_list() const
{
    return car_settings_.selected_cars();
}

ts::cup::Player_handle ts::cup::Cup::add_player(const Player& player, Player_id unique_id)
{
    Cup_player_data cup_player_data;
    cup_player_data.control_slot = player.control_slot;
    cup_player_data.handle = unique_id;
    cup_player_data.nickname = player.nickname;
    cup_player_data.color = player.color;
    cup_player_data.id = player.id;
    cup_player_data.car = (car_list().empty() ? resources::Car_handle() : car_list().front());
    cup_player_data.start_pos = player_list_.size();

    auto& data = player_map_[unique_id];
    data = cup_player_data;

    Player_handle player_handle(&data);
    player_list_.push_back(player_handle);

    if (player.control_slot != controls::invalid_slot)
    {
        local_players_.push_back(player_handle);
    }
    
    return player_handle;
}

ts::cup::Player_handle ts::cup::Cup::add_player(const Player& player)
{
    return add_player(player, allocate_player_id());
}

void ts::cup::Cup::remove_player(Player_handle player_handle)
{
    auto search_result = std::find(local_players_.begin(), local_players_.end(), player_handle);
    if (search_result != local_players_.end())
    {
        local_players_.erase(search_result);
    }

    search_result = std::find(player_list_.begin(), player_list_.end(), player_handle);
    if (search_result != player_list_.end())
    {
        player_list_.erase(search_result);
    }

    player_map_.erase(player_handle->handle);
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

const std::vector<ts::cup::Player_handle>& ts::cup::Cup::player_list() const
{
    return player_list_;
}

const std::vector<ts::cup::Player_handle>& ts::cup::Cup::action_players() const
{
    return action_players_;
}

std::size_t ts::cup::Cup::player_count() const
{
    return player_map_.size();
}

std::size_t ts::cup::Cup::max_players() const
{
    return max_players_;
}

void ts::cup::Cup::load_track_settings(const resources::Track_settings& track_settings)
{
    track_settings_ = track_settings;
}

void ts::cup::Cup::load_car_settings(const resources::Car_settings& car_settings)
{
    car_settings_ = car_settings;
}

const ts::resources::Track_settings& ts::cup::Cup::track_settings() const
{
    return track_settings_;
}

const ts::resources::Car_settings& ts::cup::Cup::car_settings() const
{
    return car_settings_;
}

void ts::cup::Cup::add_track(resources::Track_handle track_handle)
{
    track_settings_.add_track(track_handle);
}

void ts::cup::Cup::remove_track(resources::Track_handle track_handle)
{
    track_settings_.remove_track(track_handle);
}

void ts::cup::Cup::clear_tracks()
{
    track_settings_.clear_selection();
}

const std::vector<ts::resources::Track_handle>& ts::cup::Cup::track_list() const
{
    return track_settings_.selected_tracks();
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

void ts::cup::Cup::set_cup_progress(std::size_t progress)
{
    cup_progress_ = progress;
}

void ts::cup::Cup::set_player_car(Player_handle player, resources::Car_handle car_handle)
{
    auto it = player_map_.find(player->handle);
    if (it != player_map_.end())
    {
        it->second.car = car_handle;
    }
}

ts::cup::Player_handle ts::cup::Cup::get_player_by_id(Player_id player_id) const
{
    auto it = player_map_.find(player_id);
    if (it == player_map_.end())
    {
        return Player_handle();
    }

    return Player_handle(&it->second);
}

/*
ts::cup::Stage_data ts::cup::Cup::make_stage_data() const
{
    Stage_data stage_data;
    stage_data.track_handle = current_track();

    for (const auto& player_data : player_map_)
    {
        Car_data car_data;
        car_data.car = player_data.second.car;
        car_data.color = player_data.second.color;
        car_data.start_pos = player_data.second.start_pos;
        car_data.control_slot = player_data.second.control_slot;

        if (car_data.car)
        {
            stage_data.cars.push_back(car_data);
        }        
    }

    if (car_mode() == resources::Car_mode::Fixed && !car_list().empty())
    {
        for (auto& player_data : stage_data.cars)
        {
            player_data.car = car_list().front();
        }
    }

    stage_data.loaded_scripts = loaded_scripts_;
    return stage_data;
}
*/


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
        //launch_action();
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

void ts::cup::Cup::initialize_action(const Stage_data& stage_data)
{
    set_cup_state(Cup_state::Initializing);

    for (auto cup_listener : cup_listeners_)
    {
        cup_listener->on_initialize(stage_data);
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
    action_players_ = player_list_;

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
    const auto& selected_tracks = track_list();

    if (cup_progress_ < selected_tracks.size()) return selected_tracks[cup_progress_];

    return ts::resources::Track_handle();
}
