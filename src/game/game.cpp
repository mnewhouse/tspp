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
#include "game.hpp"

#include "user_interface/gui_context.hpp"

#include "states/main_menu.hpp"

#include "resources/resource_store.hpp"

#include <SFML/Graphics.hpp>

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


void ts::game::Game::main()
{
    resources::Resource_store resource_store;

    auto& video_settings = resource_store.settings.video_settings;
    auto video_mode = get_video_mode(video_settings);

    video_settings.screen_resolution.x = video_mode.width;
    video_settings.screen_resolution.y = video_mode.height;

    sf::RenderWindow render_window(video_mode, config::window_title, get_window_style(video_settings));

    gui::Context gui_context(&render_window, &resource_store.font_library);
    core::State_machine<Game_state> state_machine;    

    {
        auto main_menu = std::make_unique<states::Main_menu>(&state_machine, &gui_context, &resource_store);
        state_machine.change_state(std::move(main_menu));
    }

    state_machine.update();

    resource_store.settings.script_settings.loaded_scripts.insert("test");

    sf::Clock clock;

    std::int32_t excess_ticks = 0;

    bool vertical_sync = video_settings.vertical_sync;
    render_window.setVerticalSyncEnabled(vertical_sync);

    while (state_machine && render_window.isOpen()) 
    {
        render_window.clear();
        
        auto elapsed_ticks = clock.getElapsedTime().asMilliseconds() + excess_ticks;
        auto frame_time = elapsed_ticks / static_cast<double>(config::update_interval);

        state_machine->render(render_window, frame_time);
        gui_context.render_documents();

        render_window.display();

        elapsed_ticks = clock.getElapsedTime().asMilliseconds() + excess_ticks;
        if (elapsed_ticks >= config::update_interval)
        {
            // Update
            excess_ticks = elapsed_ticks % config::update_interval;
            auto num_updates = elapsed_ticks / config::update_interval;

            clock.restart();
            
            for (sf::Event event; render_window.pollEvent(event); ) 
            {
                if (event.type == sf::Event::Closed) 
                {
                    render_window.close();
                }

                else if (event.type == sf::Event::Resized) 
                {
                    resize_window(render_window, event.size.width, event.size.height);
                }

                gui_context.handle_event(event);
                state_machine->handle_event(event);                
            }

            gui_context.update_documents();

            for (auto i = 0; i != num_updates; ++i)
            {
                state_machine->update(config::update_interval);
            }

            state_machine.update();
        }
    }
}
