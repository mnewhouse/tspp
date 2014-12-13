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
#include "stage_interface.hpp"

const ts::resources::Loading_interface* ts::action::Stage_interface::async_load_stage(const cup::Stage_data& stage_data, std::function<Completion_callback> completion_callback)
{
    stage_loader_.set_completion_handler([this, completion_callback]()
    {
        stage_ = stage_loader_.transfer_result();
        completion_callback(stage_.get());
    });

    stage_loader_.async_load(stage_data);
    return &stage_loader_;
}

void ts::action::Stage_interface::poll()
{
    stage_loader_.poll();
}

void ts::action::Stage_interface::clear()
{
    stage_ = nullptr;
}

const ts::action::Stage* ts::action::Stage_interface::stage() const
{
    return stage_.get();
}

ts::action::Stage* ts::action::Stage_interface::stage()
{
    return stage_.get();
}