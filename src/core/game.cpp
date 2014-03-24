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
#include "handle.hpp"
#include "utility.hpp"
#include "config.hpp"

#include "states/loading_screen.hpp"
#include "user_interface/context.hpp"
#include "user_interface/event_handlers.hpp"

#include "graphics/fps_display.hpp"
#include "graphics/renderer.hpp"
#include "graphics/star_field.hpp"
#include "graphics/fonts/font_loader.hpp"
#include "graphics/fonts/font.hpp"
#include "graphics/fonts/sans.hpp"

#include "resources/resource_store.hpp"

#include <memory>

namespace
{

    void resize_window(sf::RenderWindow& render_window, int width, int height)
    {
        auto view = render_window.getView();
        view.reset(sf::FloatRect(0.0f, 0.0f, float(width), float(height)));
        render_window.setView(view);
    }

}


void ts::core::Game::main()
{
    ts::fonts::load_builtin_fonts();

    Handle<gui::Context> gui_context_handle;
    Handle<State_machine<Game_state>> state_machine_handle;

    auto& state_machine = *state_machine_handle;
    auto& gui_context = *gui_context_handle;

    auto resource_holder = std::make_shared<resources::Resource_store>();

    {
        auto star_field = std::make_shared<graphics::Star_field>(150, sf::Color(0, 0, 5));

        auto state = std::make_unique<states::Loading_state>(state_machine_handle, gui_context_handle, resource_holder);
        state->set_background(star_field);

        state_machine.change_state(std::move(state));
        state_machine.update();
    }

    sf::RenderWindow render_window(sf::VideoMode(1280, 720), "Turbo Sliders++");

    graphics::FPS_Display fps_display;

    sf::Clock clock;

    while (state_machine && render_window.isOpen()) {
        state_machine->render(render_window);
        fps_display.render(render_window);
        render_window.display();

        gui_context.reset();

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

    fonts::unload_all();
}
