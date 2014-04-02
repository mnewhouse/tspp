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

#include "gui_state.hpp"

ts::gui::State::State(state_machine_type* state_machine, Context* context, resources::Resource_store* resource_store)
    : core::Game_state(state_machine, resource_store), 
      context_(context)
{}

void ts::gui::State::set_background(const graphics::Background& background)
{
    background_ = background;
}

const ts::graphics::Background& ts::gui::State::background() const
{
    return background_;
}

void ts::gui::State::render_background(graphics::Render_target& render_target)
{
    if (background_) background_.render(render_target);
}

ts::gui::Context* ts::gui::State::context() const
{
    return context_;
}