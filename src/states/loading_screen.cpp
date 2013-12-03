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

 #include <iostream>

#include "loading_screen.hpp"
#include "main_menu.hpp"

#include "graphics/fonts/font.hpp"
#include "graphics/fonts/sans.hpp"

#include "resources/track_store.hpp"
#include "resources/car_store.hpp"

#include "core/config.hpp"

#include <thread>

void ts::states::Loading_scene::render(graphics::Render_target& render_target)
{
    sf::Text loading_text("Loading resources...",
        fonts::get_font_by_type<fonts::Sans>(), 19U);

    auto bounds = loading_text.getGlobalBounds();
    auto screen_size = render_target.getSize();

    sf::Vector2f position(screen_size.x * 0.5f, screen_size.y * 0.5f);
    position.x -= bounds.width * 0.5f;
    position.y -= bounds.height * 0.5f;

    loading_text.setPosition(position);
    render_target.draw(loading_text);
}

ts::states::Loading_state::Loading_state(const Handle<state_machine_type>& state_machine, const Handle<gui::Context>& context)
    : gui::State(state_machine, context),
      scene_(context),
      future_(async_load_resources()),
      start_time_(std::chrono::high_resolution_clock::now())
{
}

ts::states::Loading_state::~Loading_state()
{
    if (future_.valid()) future_.wait();
}

void ts::states::Loading_state::render(graphics::Render_target& render_target)
{
    render_background(render_target);
    scene_.render(render_target);
}

void ts::states::Loading_state::update(std::size_t frame_duration)
{
    auto now = std::chrono::high_resolution_clock::now();
    if (now - start_time_ > std::chrono::milliseconds(1000)) {
        if (is_loading_finished()) {
            future_.get();

            display_main_menu();
        }
    }
}

std::future<void> ts::states::Loading_state::async_load_resources()
{
    auto loader = []()
    {
        resources::Track_store track_store;
        track_store.load(config::track_root_directory);

        resources::Car_store car_store;
        car_store.load(config::car_directory);
    };

    return std::async(std::launch::async, loader);
}

bool ts::states::Loading_state::is_loading_finished() const
{
    auto status = future_.wait_for(std::chrono::seconds(0));
    return status == std::future_status::ready;
}

void ts::states::Loading_state::display_main_menu()
{
    const auto& sm_handle = state_machine();

    auto main_menu = make_unique<Main_menu>(sm_handle, context());
    main_menu->set_background(background());

    sm_handle->change_state();
    sm_handle->change_state(std::move(main_menu));
}
