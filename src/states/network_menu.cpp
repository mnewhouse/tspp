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
#include "network_menu.hpp"

#include "main_menu.hpp"
#include "cup_setup.hpp"

#include "user_interface/gui_context.hpp"
#include "user_interface/document.hpp"

#include "gui_definitions/window_template.hpp"

ts::states::Network_menu::Network_menu(Main_menu* main_menu, Cup_setup_menu* cup_setup_menu)
: main_menu_(main_menu),
  cup_setup_menu_(cup_setup_menu)
{
    create_network_menu_document(main_menu->context());
}

void ts::states::Network_menu::show()
{
    network_menu_->set_visible(true);

    cup_setup_menu_->enable_networking(this);
}

void ts::states::Network_menu::hide()
{
    network_menu_->set_visible(false);
}

void ts::states::Network_menu::return_to_main_menu()
{
    hide();

    cup_setup_menu_->disable_networking();

    main_menu_->show();
}

void ts::states::Network_menu::show_server_setup()
{
    hide();

    cup_setup_menu_->show();
}

void ts::states::Network_menu::create_network_menu_document(gui::Context* context)
{
    network_menu_ = context->create_document("network-menu");
    network_menu_->set_visible(false);
    
    auto screen_size = context->screen_size();
    Vector2i window_size(360, 400);

    auto y_position = screen_size.y - 40.0 - window_size.y;

    auto window = gui_definitions::create_styled_window(context, network_menu_.get(), window_size);
    window->set_position({ 240.0, y_position });

    gui::Text_style header_text_style;
    header_text_style.font = context->font_library().font_by_name(gui::fonts::Sans);
    header_text_style.color = sf::Color::White;
    header_text_style.character_size = 16;

    auto header_text = window->create_child<gui::Text_element>("Network Game", header_text_style);
    header_text->set_position({ 0.0, 30.0 });
    header_text->center_horizontal();

    auto text_style = header_text_style;
    text_style.character_size = 16;
    auto hover_style = text_style;
    hover_style.color = sf::Color(255, 150, 0);

    auto info_text_style = text_style;
    info_text_style.character_size = 14;
    info_text_style.color = sf::Color(200, 200, 200);

    auto server_list = window->create_child<gui::Text_element>("Server list", text_style);
    server_list->set_size({ 180.0, 24.0 });
    server_list->set_position({ 32.0, 65.0 });
    server_list->set_offset({ 8.0, 0.0 });
    server_list->register_style(gui::states::hover, hover_style);

    auto start_server = window->create_child<gui::Text_element>("Start server!", text_style);
    start_server->set_size({ 120.0, 24.0 });
    start_server->set_position({ 32.0, 90.0 });
    start_server->set_offset({ 8.0, 0.0 });
    start_server->register_style(gui::states::hover, hover_style);

    auto server_address_text = window->create_child<gui::Text_element>("Server address:", info_text_style);
    server_address_text->set_position({ 48.0, 125.0 });

    auto editbox_style = text_style;
    editbox_style.character_size = 14;

    server_address_editbox_ = window->create_child<gui::Edit_box>(editbox_style);
    server_address_editbox_->set_size({ 180.0, 24.0 });
    server_address_editbox_->set_position({ 48.0, 150.0 });
    server_address_editbox_->set_offset({ 8.0, 0.0 });
    server_address_editbox_->set_background_style(gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 20)));

    auto connect = window->create_child<gui::Text_element>("Connect!", text_style);
    connect->set_size({ 120.0, 24.0 });
    connect->set_position({ 220.0, 150.0 });
    connect->center_text_horizontal();
    connect->register_style(gui::states::hover, hover_style);

    auto recent = window->create_child<gui::Text_element>("Recent servers:", info_text_style);
    recent->set_position({ 48.0, 180.0 });

    gui::Scroll_pane_style scroll_pane_style;
    scroll_pane_style.vertical_scroll_bar_width = 16;

    auto background_style = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 15));

    auto scroll_pane = window->create_child<gui::Scroll_pane>(Vector2i(240, 100), scroll_pane_style);
    scroll_pane->set_position({ 48.0, 205.0 });
    scroll_pane->vertical_scroll_bar()->slider()->set_background_style(background_style);
    scroll_pane->vertical_scroll_bar()->set_background_style(background_style);

    gui::Vertical_list_style recent_servers_style;
    recent_servers_style.alternate_style = background_style;
    recent_servers_style.row_size = { 240.0, 20.0 };

    auto recent_server_style = editbox_style;
    auto recent_server_hover_style = recent_server_style;
    recent_server_hover_style.color = hover_style.color;

    recent_servers_list_ = scroll_pane->create_child<gui::Vertical_list<gui::Text_element>>(recent_servers_style);

    /*
    for (auto i = 0; i != 10; ++i)
    {
        const utf8_string recent_server = "localhost";
        auto row = recent_servers_list_->create_row("localhost", recent_server_style);
        row->set_offset({ 8.0, 0.0 });
        row->register_style(gui::states::hover, recent_server_hover_style);

        row->add_event_handler(gui::events::on_click, 
                               [=](const gui::Element& element)
        {
            server_address_editbox_->set_text(recent_server);
        });
    }
    */

    auto back = window->create_child<gui::Text_element>("Back", text_style);
    back->set_size({ 120.0, 24.0 });
    back->set_position({ 32.0, window_size.y - 80.0 });
    back->set_offset({ 8.0, 0.0 });
    back->register_style(gui::states::hover, hover_style);

    back->add_event_handler(gui::events::on_click, 
                            [this](const gui::Element&)
    {
        return_to_main_menu();
    });

    back->add_event_handler(gui::events::on_key_down,
                            [this](const gui::Element&, sf::Keyboard::Key key, unsigned key_modifiers)
    {
        if (key == sf::Keyboard::Escape)
        {
            return_to_main_menu();
        }
    });

    start_server->add_event_handler(gui::events::on_click,
                                    [this](const gui::Element&)
    {
        show_server_setup();
    });
}