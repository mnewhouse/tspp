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
#include "main_menu.hpp"

#include "player_setup.hpp"
#include "cup_setup.hpp"
#include "settings_menu.hpp"
#include "network_menu.hpp"

#include "gui_definitions/background_document.hpp"
#include "gui_definitions/window_template.hpp"

ts::states::Main_menu::Main_menu(state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store)
: gui::State(state_machine, context, resource_store)
{
    background_document_ = gui_definitions::create_background_document(context);

    player_setup_ = std::make_unique<Player_setup_menu>(this);
    cup_setup_ = std::make_unique<Cup_setup_menu>(this);
    settings_menu_ = std::make_unique<Settings_menu>(this);
    network_game_menu_ = std::make_unique<Network_menu>(this, cup_setup_.get());    

    create_main_document(context);
}

void ts::states::Main_menu::on_activate()
{
    player_setup_->hide();
    cup_setup_->hide();

    background_document_->set_visible(true);
    main_document_->set_visible(true);
}

void ts::states::Main_menu::create_main_document(gui::Context* context)
{
    auto& font_library = context->font_library();
    auto screen_size = context->screen_size();

    main_document_ = context->create_document("main-menu");

    gui::Text_style text_style;
    text_style.font = font_library.font_by_name(gui::fonts::Sans);
    text_style.color = sf::Color::White;
    text_style.character_size = 16;

    gui::Text_style hover_style = text_style;
    hover_style.color = sf::Color(255, 150, 0);

    gui::Vertical_list_style list_style;
    list_style.row_size = { 200.0, 24.0 };

    auto container = gui_definitions::create_styled_window(context, main_document_.get(), Vector2i(200, 240));
    container->set_position({ 40.0, screen_size.y - 280.0 });

    gui::Text_element::Styler_type text_styler(text_style);
    text_styler.register_style(gui::states::hover, hover_style);

    using navigation_list = gui::Vertical_list<gui::Text_element>;

    using namespace gui::events;    

    auto nav = container->create_child<navigation_list>(list_style);
    nav->set_position({ 40.0, 30.0 });

    auto players = nav->create_row("Players", text_styler);
    players->add_event_handler(on_click, 
        [this](gui::Element&) 
    { 
        show_player_setup(); 
    });

    auto local_game = nav->create_row("Local Game", text_styler);
    local_game->add_event_handler(on_click,
        [this](gui::Element& element)
    {
        show_local_cup_setup();
    });

    auto network_game = nav->create_row("Network Game", text_styler);
    network_game->add_event_handler(on_click, 
                                    [this](const gui::Element&)
    {
        show_network_game_menu();
    });
    

    nav->create_row("Editor", text_styler);
    auto settings = nav->create_row("Settings", text_styler);
    settings->add_event_handler(on_click, 
                                [this](const gui::Element&)
    {
        show_settings_menu();
    });


    auto quit_button = nav->create_row("Quit", text_styler);
    quit_button->add_event_handler(on_click, 
        [this](gui::Element&) 
    { 
        quit(); 
    });
}


void ts::states::Main_menu::show()
{
    main_document_->set_visible(true);
}

void ts::states::Main_menu::quit()
{
    state_machine()->clear();
}

void ts::states::Main_menu::show_local_cup_setup()
{
    main_document_->set_visible(false);

    cup_setup_->show();
}

void ts::states::Main_menu::show_player_setup()
{
    main_document_->set_visible(false);

    player_setup_->show();
}

void ts::states::Main_menu::show_settings_menu()
{
    main_document_->set_visible(false);

    settings_menu_->show();
}

void ts::states::Main_menu::show_network_game_menu()
{
    main_document_->set_visible(false);

    network_game_menu_->show();
}