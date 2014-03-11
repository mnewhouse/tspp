/*
 * Turbo Sliders++
 * Copyright (C) 2013 Martin Newhouse
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

ts::cup::Cup::Cup() : 
    current_track_(0)
{
}

void ts::cup::Cup::make_stage_data()
{
    auto count = player_count();
    stage_data_.players.resize(count);
    for (std::size_t id = 0; id != count; ++id)
    {
        stage_data_.players[id].car = car_list_.front();
        stage_data_.players[id].control_slot = player_list_[id].control_slot;
    }
}

void ts::cup::Cup::add_player(const Player& player)
{
    player_list_.push_back(player);
}

std::size_t ts::cup::Cup::player_count() const
{
    return player_list_.size();
}

const ts::cup::Stage_data& ts::cup::Cup::stage_data() const
{
    return stage_data_;
}

void ts::cup::Cup::add_track(resources::Track_handle track_handle)
{
    track_list_.push_back(std::move(track_handle));
}

void ts::cup::Cup::set_car(const resources::Car_definition& car_def)
{
    car_list_.clear();
    car_list_.push_back(car_def);

    car_mode_ = Car_mode::Fixed;
}

void ts::cup::Cup::add_car(const resources::Car_definition& car_def)
{
    car_list_.push_back(car_def);

    if (car_mode_ == Car_mode::Fixed) {
        car_mode_ = Car_mode::Free;
    }
}

void ts::cup::Cup::set_car_mode(Car_mode car_mode)
{
    car_mode_ = car_mode;
}

const std::vector<ts::resources::Car_definition>& ts::cup::Cup::car_list() const
{
    return car_list_;
}

const std::vector<ts::resources::Track_handle>& ts::cup::Cup::track_list() const
{
    return track_list_;
}

void ts::cup::Cup::next_track()
{
    ++current_track_;
}

ts::resources::Track_handle ts::cup::Cup::current_track() const
{
    if (current_track_ < track_list_.size()) return track_list_[current_track_];

    return ts::resources::Track_handle();
}