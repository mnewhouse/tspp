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

#include <iostream>

#include "game.hpp"
#include "utility.hpp"
#include "config.hpp"

#include "states/loading_screen.hpp"
#include "user_interface/context.hpp"
#include "user_interface/event_handlers.hpp"

#include "graphics/renderer.hpp"
#include "graphics/star_field.hpp"
#include "graphics/fonts/font_loader.hpp"
#include "graphics/fonts/font.hpp"
#include "graphics/fonts/sans.hpp"
#include "graphics/fps_display.hpp"

#include "resources/resource_store.hpp"

#include <memory>

namespace ts
{

    void resize_window(sf::RenderWindow& render_window, int width, int height)
    {
        auto view = render_window.getView();
        view.reset(sf::FloatRect(0.0f, 0.0f, float(width), float(height)));
        render_window.setView(view);
    }

    sf::VideoMode get_video_mode(const resources::Video_settings& video_settings)
    {
        sf::VideoMode video_mode(video_settings.screen_resolution.x, video_settings.screen_resolution.y);

        if (video_settings.full_screen)
        {
            if (!video_mode.isValid())
            {
                const auto& fullscreen_modes = sf::VideoMode::getFullscreenModes();
                video_mode = fullscreen_modes.front();
            }
        }

        else
        {
            auto desktop_mode = sf::VideoMode::getDesktopMode();
            if (video_mode.width == 0 || video_mode.height == 0 ||
                video_mode.width > desktop_mode.width || video_mode.height > desktop_mode.height)
            {
                video_mode.width = 640;
                video_mode.height = 480;
            }
        }

        return video_mode;
    }

    std::uint32_t get_window_style(const resources::Video_settings& video_settings)
    {
        if (video_settings.full_screen) return sf::Style::Fullscreen;

        return sf::Style::Titlebar;
    }
}


void ts::core::Game::main()
{
    ts::fonts::load_builtin_fonts();

    resources::Resource_store resource_store;
    resource_store.settings = resources::load_settings(config::settings_file);

    auto& video_settings = resource_store.settings.video_settings;
    auto video_mode = get_video_mode(video_settings);

    video_settings.screen_resolution.x = video_mode.width;
    video_settings.screen_resolution.y = video_mode.height;

    sf::RenderWindow render_window(video_mode, config::window_title, get_window_style(video_settings));

    gui::Context gui_context(Vector2u(video_mode.width, video_mode.height));
    State_machine<Game_state> state_machine;

    {
        auto star_field = std::make_shared<graphics::Star_field>(150, sf::Color(0, 0, 5));

        auto state = std::make_unique<states::Loading_state>(&state_machine, &gui_context, &resource_store);
        state->set_background(star_field);

        state_machine.change_state(std::move(state));
        state_machine.update();
    }

    sf::Clock clock;
    graphics::FPS_Display fps_display;

    while (state_machine && render_window.isOpen()) {
        state_machine->render(render_window);
        fps_display.render(render_window);
        render_window.display();

        if (clock.getElapsedTime().asMilliseconds() >= 16) {
            for (sf::Event event; render_window.pollEvent(event);) {
                if (event.type == sf::Event::Closed) {
                    render_window.close();
                }

                else if (event.type == sf::Event::Resized) {
                    resize_window(render_window, event.size.width, event.size.height);
                }

                state_machine->handle_event(event);

                auto new_mouse_state = gui::events::mouse_event(gui_context.mouse_state(), event);
                gui_context.set_mouse_state(new_mouse_state);
            }

            clock.restart();

            state_machine->update(16);
        }

        state_machine.update();
    }    

    state_machine.clear_now();

    resources::save_settings(config::settings_file, resource_store.settings);

    fonts::unload_all();
}
