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

#include "cup.hpp"
#include "player.hpp"

ts::game::Cup::Cup() : 
    current_track_(0)
{
}

void ts::game::Cup::make_stage_data()
{
    auto count = player_count();
    stage_data_.players.resize(count);

    std::size_t id = 0;
    for (auto& player : stage_data_.players)
    {
        player.car = car_list_.front();
        player.control_slot = player_list_[id].control_slot;
        player.start_pos = id;

        ++id;
    }
}

void ts::game::Cup::add_player(const Player& player)
{
    player_list_.push_back(player);
}

std::size_t ts::game::Cup::player_count() const
{
    return player_list_.size();
}

const ts::game::Stage_data& ts::game::Cup::stage_data() const
{
    return stage_data_;
}

void ts::game::Cup::add_track(resources::Track_handle track_handle)
{
    track_list_.push_back(track_handle);
}

void ts::game::Cup::set_car(const resources::Car_definition& car_def)
{
    car_list_.clear();
    car_list_.push_back(car_def);

    car_mode_ = Car_mode::Fixed;
}

void ts::game::Cup::add_car(const resources::Car_definition& car_def)
{
    car_list_.push_back(car_def);

    if (car_mode_ == Car_mode::Fixed) {
        car_mode_ = Car_mode::Free;
    }
}

void ts::game::Cup::set_car_mode(Car_mode car_mode)
{
    car_mode_ = car_mode;
}

const std::vector<ts::resources::Car_definition>& ts::game::Cup::car_list() const
{
    return car_list_;
}

const std::vector<ts::resources::Track_handle>& ts::game::Cup::track_list() const
{
    return track_list_;
}

void ts::game::Cup::next_track()
{
    ++current_track_;
}

ts::resources::Track_handle ts::game::Cup::current_track() const
{
    if (current_track_ < track_list_.size()) return track_list_[current_track_];

    return ts::resources::Track_handle();
}