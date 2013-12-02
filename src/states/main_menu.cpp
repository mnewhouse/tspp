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

#include "main_menu.hpp"
#include "graphics/fonts/sans.hpp"
#include "graphics/fonts/font.hpp"

#include "user_interface/elements/button.hpp"
#include "user_interface/elements/rectangle_region.hpp"

#include "resources/track_store.hpp"
#include "resources/track.hpp"

#include "resources/car_store.hpp"
#include "resources/pattern.hpp"

#include "local_cup.hpp"

#include "core/utility.hpp"



ts::states::Main_menu::Main_menu(const Handle<state_machine_type>& state_machine, const Handle<gui::Context>& gui_context)
    : gui::State(state_machine, gui_context),
      scene_(this, gui_context)
{
}

void ts::states::Main_menu::render(graphics::Render_target& render_target)
{
    render_background(render_target);
    scene_.render(render_target);
}

void ts::states::Main_menu::update(std::size_t frame_duration)
{
}

void ts::states::Main_menu_scene::render(graphics::Render_target& render_target)
{
    typedef gui::elements::Button<gui::elements::Rectangle_region<float>> rectangular_button;

    auto screen_size = render_target.getSize();
    Float_rect rect(40.f, screen_size.y - 240.f, 200.f, 32.f);

    gui::element_id id = 0;

    auto menu_item = [&](const std::string& text)
    {
        sf::Text text_item(text, fonts::get_font_by_type<fonts::Sans>(), 24);
        text_item.setPosition(rect.left + 5, rect.top);
        text_item.setColor(sf::Color::White);

        rectangular_button button(make_id(++id), rect);
        gui::Element_state button_state = button(context());

        if (button_state.hot) {
            text_item.setColor(sf::Color(255, 150, 0));
        }

        render_target.draw(text_item);

        rect.top += rect.height;
        return button_state;
    };

    if (menu_item("Play").clicked) {
        resources::Track_store track_store;
        resources::Car_store car_store;

        const auto& state_machine = main_menu_->state_machine();

        auto cup_state = make_unique<Local_cup_state>(state_machine, context());
        cup_state->set_background(main_menu_->background());

        cup::Player player;
        player.nickname = "Iziwyn";
        player.control_slot = 1;

        cup_state->add_player(player);

        for (std::size_t n = 0; n != 1; ++n) {
            cup_state->add_track(track_store->get_track_by_name("clockwise"));
        }

        auto car = car_store->get_car_by_name("tspptest");
        if (car) {
            cup_state->set_car(*car);
            state_machine->change_state(std::move(cup_state));
        }        
    }

    if (menu_item("Editor").clicked) {

    }

    if (menu_item("Settings").clicked) {

    }

    if (menu_item("Quit").clicked) {

        main_menu_->state_machine()->clear();
    }

}