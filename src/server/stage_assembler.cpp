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
#include "stage_assembler.hpp"

#include "cup/cup_listener.hpp"
#include "cup/cup_controller.hpp"
#include "cup/stage_data.hpp"

struct ts::server::Stage_assembler::Impl
    : private cup::Scoped_cup_listener
{
    Impl(Stage_assembler* self, cup::Cup_controller* cup_controller);
    ~Impl();

    std::uint32_t add_car(const resources::Player_definition& player_definition, const resources::Car_handle& car_handle,
                          cup::Player_handle controller = cup::Player_handle(), std::int32_t start_pos = -1);

    void remove_car_by_id(std::uint32_t car_id);

    const cup::Stage_data& initialize_stage_data();

    virtual void on_state_change(cup::Cup_state old_state, cup::Cup_state new_state) override;

    cup::Stage_data stage_data_;
    std::list<Stage_assembler::Modifier> modifiers_;

    Stage_assembler* self_ = nullptr;
    cup::Cup_controller* cup_controller_ = nullptr;    

    std::uint32_t car_id_ = 0;    
};

ts::server::Stage_assembler::Impl::Impl(Stage_assembler* self, cup::Cup_controller* cup_controller)
: Scoped_listener<cup::Cup_listener>(cup_controller->cup_listener_host()),
  self_(self),
  cup_controller_(cup_controller)
{
}

ts::server::Stage_assembler::Impl::~Impl()
{
}

void ts::server::Stage_assembler::Impl::on_state_change(cup::Cup_state old_state, cup::Cup_state new_state)
{
    if (new_state == cup::Cup_state::Awaiting_initialization)
    {
        initialize_stage_data();

        // Apply the stage data modifiers
        for (const auto& modifier : modifiers_)
        {
            modifier(*self_);
        }        

        cup_controller_->initialize_action(stage_data_);
    }
}

const ts::cup::Stage_data& ts::server::Stage_assembler::Impl::initialize_stage_data()
{
    stage_data_ = {};

    stage_data_.track = cup_controller_->current_track();
    stage_data_.script_resources = cup_controller_->loaded_resources();

    car_id_ = 0;
    for (const auto& stage_player : cup_controller_->stage_players())
    {
        const auto& player = stage_player.player;
        const auto& car = stage_player.car;

        if (player)
        {
            // This can proceed even if there is no car available, because it might be set later.
            // And if it's not set, we have to start without this player.

            resources::Player_definition player_def;
            player_def.name = player->nickname;
            player_def.id = player->id;

            player_def.color = player->color;

            add_car(player_def, car, player, stage_player.start_pos);
        }
    }    

    return stage_data_;
}

std::uint32_t ts::server::Stage_assembler::Impl::add_car(const resources::Player_definition& player_definition, const resources::Car_handle& car_handle,
                                                         cup::Player_handle controller, std::int32_t start_pos)
{
    auto& cars = stage_data_.cars;
    cars.emplace_back();
    auto& car_data = cars.back();

    car_data.car_def = car_handle;
    car_data.controller = controller;
    car_data.player = player_definition;
    car_data.start_pos = start_pos;
    car_data.car_id = ++car_id_;

    auto conflicting_pos = std::find_if(cars.begin(), cars.end(), 
                                        [start_pos](const cup::Car_data& car_data)
    {
        return start_pos == car_data.start_pos;
    });

    if (conflicting_pos != cars.end() || start_pos == -1)
    {
        auto max_pos = std::max_element(cars.begin(), cars.end(), 
                                       [](const cup::Car_data& a, const cup::Car_data& b)
        {
            return a.start_pos < b.start_pos;
        });

        if (max_pos != cars.end())
        {
            car_data.start_pos = max_pos->start_pos + 1;
        }

        else
        {
            car_data.start_pos = 0;
        }
    }

    return car_data.car_id;
}

void ts::server::Stage_assembler::Impl::remove_car_by_id(std::uint32_t car_id)
{
    auto& cars = stage_data_.cars;
    cars.erase(std::remove_if(cars.begin(), cars.end(), 
        [car_id](const cup::Car_data& car_data)
    {
        return car_data.car_id == car_id;
    }), cars.end());
}

ts::server::Stage_assembler::Stage_assembler(cup::Cup_controller* cup_controller)
: impl_(std::make_unique<Impl>(this, cup_controller))
{
}

ts::server::Stage_assembler::~Stage_assembler()
{
}

std::uint32_t ts::server::Stage_assembler::add_car(const resources::Player_definition& player_definition, const resources::Car_handle& car_handle,
                                                   cup::Player_handle controller, std::int32_t start_pos)
{
    return impl_->add_car(player_definition, car_handle, controller, start_pos);
}

void ts::server::Stage_assembler::remove_car_by_id(std::uint32_t car_id)
{
    impl_->remove_car_by_id(car_id);
}

const ts::cup::Stage_data& ts::server::Stage_assembler::stage_data() const
{
    return impl_->stage_data_;
}

ts::server::Stage_assembler::Modifier_handle ts::server::Stage_assembler::add_modifier(Modifier modifier)
{
    impl_->modifiers_.push_back(std::move(modifier));
    return std::prev(impl_->modifiers_.end());
}

void ts::server::Stage_assembler::remove_modifier(Modifier_handle modifier_handle)
{
    impl_->modifiers_.erase(modifier_handle);
}