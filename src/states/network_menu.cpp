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
#include "client_cup_state.hpp"

#include "user_interface/gui_context.hpp"
#include "user_interface/document.hpp"

#include "gui_definitions/window_template.hpp"

#include "resources/settings/network_settings.hpp"

namespace ts
{
    std::pair<utf8_string, std::uint16_t> extract_port_from_address(utf8_string address, std::uint16_t default_port);
}

ts::states::Network_menu::Network_menu(Main_menu* main_menu, Cup_setup_menu* cup_setup_menu)
: main_menu_(main_menu),
  cup_setup_menu_(cup_setup_menu)
{
    create_network_menu_document(main_menu->context());

    connection_dialog_ = std::make_unique<Connection_dialog>(main_menu->context(), this);
}

ts::states::Network_menu::~Network_menu()
{
}

void ts::states::Network_menu::show()
{
    network_menu_->set_visible(true);
    network_menu_->set_active(true);

    server_address_editbox_->set_active(true);

    cup_setup_menu_->enable_networking(this);
}

void ts::states::Network_menu::hide()
{
    network_menu_->set_visible(false);
}

void ts::states::Network_menu::update(std::size_t frame_duration)
{
    if (client_cup_state_)
    {
        // We are currently connecting
        using client::Connection_status;
        using client::Registration_status;

        client_cup_state_->update(frame_duration);

        if (client_cup_state_->connection_status() == Connection_status::Failed)
        {
            client_cup_state_ = {};
            connection_dialog_->set_status_text("Connection failed.");
            registration_status_ = client::Registration_status::None;
        }

        else if (client_cup_state_->connection_status() == Connection_status::Connected)
        {
            if (registration_status_ == Registration_status::None)
            {
                client_cup_state_->send_registration_request();
                connection_dialog_->set_status_text("Registering...");
                registration_status_ = client_cup_state_->registration_status();
            }

            auto reg_status = client_cup_state_->registration_status();
            if (reg_status != registration_status_)
            {
                switch (reg_status)
                {
                case Registration_status::Registered:
                    activate_cup_state();
                    break;
                
                case Registration_status::Error:
                    handle_registration_error(client_cup_state_->registration_error());
                    break;
                }
            }
        }
    }
}

void ts::states::Network_menu::activate_cup_state()
{
    auto state_machine = main_menu_->state_machine();
    state_machine->change_state(std::move(client_cup_state_));

    hide();

    connection_dialog_->hide();
}

void ts::states::Network_menu::handle_registration_error(const utf8_string& error_message)
{
    connection_dialog_->set_status_text("Error: " + error_message);

    client_cup_state_ = {};
    registration_status_ = client::Registration_status::None;
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

void ts::states::Network_menu::cancel_connect_attempt()
{
    client_cup_state_ = {};
    connection_dialog_->hide();

    show();
}

void ts::states::Network_menu::initiate_client_connection(utf8_string address_string)
{
    if (!client_cup_state_)
    {
        client_cup_state_ = std::make_unique<Client_cup_state>(main_menu_->state_machine(),
                                                               main_menu_->context(),
                                                               main_menu_->resource_store());

        auto& network_settings = main_menu_->resource_store()->network_settings();

        auto address = extract_port_from_address(std::move(address_string), network_settings.server_port);

        client_cup_state_->async_connect(address.first, address.second);

        connection_dialog_->set_status_text("Connecting...");
        connection_dialog_->show(client_cup_state_.get());

        network_menu_->set_active(false);
        server_address_editbox_->set_active(false);

        registration_status_ = client::Registration_status::None;
    }
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

    connect->add_event_handler(gui::events::on_click,
                               [this](const gui::Element& element)
    {
        initiate_client_connection(server_address_editbox_->text());
    });
}

ts::states::Connection_dialog::Connection_dialog(gui::Context* context, Network_menu* network_menu)
: network_menu_(network_menu)
{
    create_connection_dialog_document(context);
}

void ts::states::Connection_dialog::show(Client_cup_state* client_cup_state)
{
    connection_dialog_->set_visible(true);

    client_cup_state_ = client_cup_state;
}

void ts::states::Connection_dialog::hide()
{
    connection_dialog_->set_visible(false);
}

void ts::states::Connection_dialog::set_status_text(utf8_string status_text)
{
    connection_state_text_->set_text(std::move(status_text));
}

void ts::states::Connection_dialog::cancel_connect_attempt()
{
    network_menu_->cancel_connect_attempt();

    client_cup_state_ = nullptr;
}

std::pair<ts::utf8_string, std::uint16_t> ts::extract_port_from_address(utf8_string address, std::uint16_t default_port)
{
    std::uint32_t colon[] = { ':' };

    auto it = std::find_end(address.begin(), address.end(), std::begin(colon), std::end(colon));
    if (it != address.end())
    {
        std::stringstream stream;
        std::copy(std::next(it), address.end(), std::ostreambuf_iterator<char>(stream));

        std::uint16_t port;
        if (stream >> port)
        {
            address.erase(it, address.end());
            return std::make_pair(std::move(address), port);
        }
    }

    return std::make_pair(std::move(address), default_port);
}

void ts::states::Connection_dialog::create_connection_dialog_document(gui::Context* context)
{
    connection_dialog_ = context->create_document("connect-dialog");
    connection_dialog_->set_background_style(gui::make_background_style<gui::Plain_background>(sf::Color(0, 0, 0, 128)));

    Vector2i dialog_size(320, 120);

    auto dialog = gui_definitions::create_styled_window(context, connection_dialog_.get(), dialog_size);
    dialog->center_horizontal();
    dialog->center_vertical();

    gui::Text_style text_style;
    text_style.font = context->font_library().font_by_name(gui::fonts::Sans);
    text_style.character_size = 16;
    text_style.color = sf::Color(180, 180, 180);

    auto button_style = text_style;
    auto hover_style = text_style;
    button_style.color = sf::Color(255, 255, 255);
    hover_style.color = sf::Color(255, 150, 0);

    connection_state_text_ = dialog->create_child<gui::Text_element>(utf8_string(), text_style);
    connection_state_text_->set_position({ 0.0, 30.0 });
    connection_state_text_->center_horizontal();

    cancel_button_ = dialog->create_child<gui::Text_element>("Cancel", button_style);
    cancel_button_->set_size({ 80.0, 25.0 });
    cancel_button_->set_position({ 0.0, 60.0 });
    cancel_button_->center_horizontal();
    cancel_button_->center_text_horizontal();
    cancel_button_->register_style(gui::states::hover, hover_style);

    cancel_button_->add_event_handler(gui::events::on_click, 
                                      [this](const gui::Element&)
    {
        cancel_connect_attempt();
    });

    cancel_button_->add_event_handler(gui::events::on_key_down,
                                      [this](const gui::Element&, sf::Keyboard::Key key, unsigned int key_modifiers)
    {
        if (key == sf::Keyboard::Escape || key == sf::Keyboard::Return)
        {
            cancel_connect_attempt();
        }
    });

    connection_dialog_->set_visible(false);
}