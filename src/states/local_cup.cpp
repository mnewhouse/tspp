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

#include "local_cup.hpp"

#include "graphics/fonts/sans.hpp"
#include "graphics/fonts/font.hpp"

#include "user_interface/elements/button.hpp"
#include "user_interface/elements/rectangle_region.hpp"

#include "action_state.hpp"

#include <iostream>

ts::states::Local_cup_state::Local_cup_state(const Handle<state_machine_type>& state_machine, const Handle<gui::Context>& context, 
                                             std::shared_ptr<resources::Resource_store> resource_store)

    : Cup_state(state_machine, context, std::move(resource_store)),
      scene_(this, context)
{
}

void ts::states::Local_cup_state::update(std::size_t)
{
    if (is_loading() && is_loading_finished()) {
        try {
            launch_action();
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
            next_track();
        }

        catch (...) {
            std::cout << "Unknown exception." << std::endl;
        }
    }
}

void ts::states::Local_cup_state::render(graphics::Render_target& render_target)
{
    render_background(render_target);
    scene_.render(render_target);
}

ts::states::Local_cup_scene::Local_cup_scene(Local_cup_state* cup_state, const Handle<gui::Context>& context)
    : gui::Scene(context), 
      cup_state_(cup_state)
{
}

void ts::states::Local_cup_scene::render(graphics::Render_target& render_target)
{
    auto window_size = render_target.getSize();

    Float_rect rect(0, 0, float(window_size.x), float(window_size.y));

    const auto track_handle = cup_state_->current_track();
    const auto& font = fonts::get_font_by_type<fonts::Sans>();

    if (track_handle) {
        sf::Text text(track_handle.name(), font, 30U);
        text.setPosition(200.f, 200.f);
        render_target.draw(text);
    }

    gui::elements::Rectangular_button<float> button(make_id(0), rect);
    if (button(context()).clicked) {
        if (track_handle && !cup_state_->is_loading()) {
            cup_state_->async_load();
        }

        else {
            cup_state_->state_machine()->change_state();
        }
    }
}

