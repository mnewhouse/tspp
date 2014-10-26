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

ts::cup::Cup::Cup(Locality locality)
: cup_progress_(0),
  locality_(locality),
  state_(Cup_state::Registering)
{
}

ts::cup::Cup::~Cup()
{
}

bool ts::cup::Cup::is_local() const
{
    return locality_ == Locality::Local;
}

ts::cup::Cup_state ts::cup::Cup::cup_state() const
{
    return state_;
}

std::size_t ts::cup::Cup::cup_progress() const
{
    return cup_progress_;
}

std::size_t ts::cup::Cup::stage_count() const
{
    return stage_count_;
}

ts::cup::Player_handle ts::cup::Cup::add_player(const Player& player, Player_id unique_id)
{    
    auto& player_def = player_map_[unique_id];
    player_def.handle = unique_id;
    static_cast<Player&>(player_def) = player;

    Player_handle player_handle(&player_def);
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

void ts::cup::Cup::clear_players()
{
    local_players_.clear();
    player_list_.clear();
    action_players_.clear();

    player_map_.clear();
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

const std::vector<ts::cup::Player_handle>& ts::cup::Cup::player_list() const
{
    return player_list_;
}

const std::vector<ts::cup::Player_handle>& ts::cup::Cup::action_players() const
{
    return action_players_;
}

const std::vector<ts::cup::Player_handle>& ts::cup::Cup::local_players() const
{
    return local_players_;
}

std::size_t ts::cup::Cup::player_count() const
{
    return player_map_.size();
}

void ts::cup::Cup::add_cup_listener(Cup_listener* cup_listener)
{
    cup_listeners_.push_back(cup_listener);
}

void ts::cup::Cup::remove_cup_listener(Cup_listener* cup_listener)
{
    cup_listeners_.erase(std::remove(cup_listeners_.begin(), cup_listeners_.end(), cup_listener), cup_listeners_.end());
}

void ts::cup::Cup::set_cup_state(Cup_state new_state)
{
    if (new_state != state_)
    {
        auto old_state = state_;
        state_ = new_state;

        for (auto listener : cup_listeners_)
        {
            listener->on_state_change(old_state, state_);
        }        
    }
}

void ts::cup::Cup::set_stage_count(std::size_t stage_count)
{
    stage_count_ = stage_count;
}

void ts::cup::Cup::set_cup_progress(std::size_t progress)
{
    cup_progress_ = progress;
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

void ts::cup::Cup::initialize_action(const Stage_data& stage_data)
{
    for (auto cup_listener : cup_listeners_)
    {
        cup_listener->on_initialize(stage_data);
    }   
}
