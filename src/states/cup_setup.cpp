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
#include "cup_setup.hpp"

#include "server_cup_state.hpp"
#include "network_menu.hpp"

#include "user_interface/elements/elements.hpp"
#include "gui_definitions/window_template.hpp"

#include "resources/car_store.hpp"

#include "resources/settings/track_settings.hpp"
#include "resources/settings/car_settings.hpp"
#include "resources/settings/network_settings.hpp"

ts::states::Cup_setup_menu::Cup_setup_menu(Main_menu* main_menu)
: main_menu_(main_menu),
  network_menu_(nullptr)
{
    create_navigation_document(main_menu->context());

    track_setup_menu_ = std::make_unique<Track_setup_menu>(this);
    car_setup_menu_ = std::make_unique<Car_setup_menu>(this);

    check_startability();
}

void ts::states::Cup_setup_menu::start_cup()
{
    // Need to create cup state
    auto state_machine = main_menu_->state_machine();

    auto cup_state = std::make_unique<Server_cup_state>(state_machine, context(), resource_store());
    if (network_menu_)
    {
        cup_state->listen(resource_store()->network_settings().server_port);
    }

    state_machine->change_state(std::move(cup_state));


    navigation_document_->set_visible(false);

    track_setup_menu_->hide();
    car_setup_menu_->hide();
}

void ts::states::Cup_setup_menu::return_to_main_menu()
{
    navigation_document_->set_visible(false);

    track_setup_menu_->hide();
    car_setup_menu_->hide();

    if (network_menu_)
    {
        network_menu_->show();
    }

    else
    {
        main_menu_->show();
    }
}

void ts::states::Cup_setup_menu::enable_networking(Network_menu* network_menu)
{
    network_menu_ = network_menu;

    network_button_->set_active(true);
}

void ts::states::Cup_setup_menu::disable_networking()
{
    network_menu_ = nullptr;

    network_button_->set_active(false);
}

ts::resources::Resource_store* ts::states::Cup_setup_menu::resource_store()
{
    return main_menu_->resource_store();
}

ts::gui::Context* ts::states::Cup_setup_menu::context()
{
    return main_menu_->context();
}

void ts::states::Cup_setup_menu::show()
{
    navigation_document_->set_visible(true);

    show_track_setup();
}

void ts::states::Cup_setup_menu::hide()
{
    navigation_document_->set_visible(false);

    car_setup_menu_->hide();
    track_setup_menu_->hide();
}

void ts::states::Cup_setup_menu::show_track_setup()
{
    car_setup_menu_->hide();

    track_setup_menu_->show();
}

void ts::states::Cup_setup_menu::show_car_setup()
{
    track_setup_menu_->hide();

    car_setup_menu_->show();
}

ts::gui::Element* ts::states::Cup_setup_menu::create_containment_window(gui::Document* document, utf8_string title)
{
    auto context = main_menu_->context();
    auto screen_size = context->screen_size();

    auto& font_library = context->font_library();

    auto width = std::max(screen_size.x - 320, 360);
    auto height = std::max(screen_size.y - 120, 440);

    auto y_position = screen_size.y - 40.0 - height;

    Vector2<double> window_size(width, height);

    auto window = gui_definitions::create_styled_window(context, document, window_size);
    window->set_position({ 240.0, y_position });

    gui::Text_style title_style;
    title_style.character_size = 16;
    title_style.color = sf::Color::White;
    title_style.font = font_library.font_by_name(gui::fonts::Sans);

    auto title_text = window->create_child<gui::Text_element>(std::move(title), title_style);
    title_text->center_horizontal();
    title_text->set_position({ 0.0, 25.0 });

    return window;
}


void ts::states::Cup_setup_menu::create_navigation_document(gui::Context* context)
{
    auto& font_library = context->font_library();
    auto screen_size = context->screen_size();

    navigation_document_ = context->create_document("cup-setup-navigation");

    gui::Text_style text_style;
    text_style.font = font_library.font_by_name(gui::fonts::Sans);
    text_style.color = sf::Color::White;
    text_style.character_size = 16;

    gui::Text_style hover_style = text_style;
    hover_style.color = sf::Color(255, 150, 0);

    gui::Text_style inactive_style = text_style;
    inactive_style.color = sf::Color(150, 150, 150);

    gui::Vertical_list_style list_style;
    list_style.row_size = { 200.0, 24.0 };

    auto container = gui_definitions::create_styled_window(context, navigation_document_.get(), Vector2i(200, 216));
    container->set_position({ 40.0, screen_size.y - 280.0 });

    gui::Text_element::Styler_type text_styler(text_style);
    text_styler.register_style(gui::states::inactive, inactive_style);
    text_styler.register_style(gui::states::hover, hover_style);

    using navigation_list = gui::Vertical_list<gui::Text_element>;

    using namespace gui::events;

    auto nav = container->create_child<navigation_list>(list_style);
    nav->set_position({ 40.0, 30.0 });

    auto tracks = nav->create_row("Tracks", text_styler);
    tracks->add_event_handler(on_click, 
        [this](const gui::Element&) 
    {
        show_track_setup();
    });

    
    auto cars = nav->create_row("Cars", text_styler);
    cars->add_event_handler(on_click, 
        [this](const gui::Element& element)
    {
        show_car_setup();
    });

    auto resources = nav->create_row("Resources", text_styler);
    resources->add_event_handler(on_click,
        [this](const gui::Element& element)
    {

    });


    network_button_ = nav->create_row("Network", text_styler);
    network_button_->add_event_handler(on_click,
        [this](const gui::Element& element)
    {

    });

    network_button_->set_active(network_menu_ != nullptr);

    start_button_ = nav->create_row("Start!", text_styler);
    start_button_->add_event_handler(on_click, 
        [this](const gui::Element& element)
    {
        start_cup();
    });

    auto quit_button = nav->create_row("Back", text_styler);
    quit_button->add_event_handler(on_click, 
        [this](const gui::Element&)
    { 
        return_to_main_menu(); 
    });

    navigation_document_->set_visible(false);
}

void ts::states::Cup_setup_menu::check_startability()
{
    auto& track_settings = main_menu_->resource_store()->track_settings();
    auto& car_settings = main_menu_->resource_store()->car_settings();

    bool startable = !car_settings.selected_cars().empty() && !track_settings.selected_tracks().empty();

    start_button_->set_active(startable);
}

ts::states::Track_setup_menu::Track_setup_menu(Cup_setup_menu* cup_setup_menu)
: cup_setup_menu_(cup_setup_menu),
  current_directory_(cup_setup_menu->resource_store()->track_store().root_directory()),
  track_setup_document_(create_track_setup_document(cup_setup_menu)),
  random_selection_dialog_(create_random_selection_dialog(cup_setup_menu))  
{
}

ts::gui::Document_handle ts::states::Track_setup_menu::create_random_selection_dialog(Cup_setup_menu* cup_setup_menu)
{
    auto context = cup_setup_menu->context();

    auto& font_library = context->font_library();

    auto document = context->create_document("random-selection-dialog");

    auto window = gui_definitions::create_styled_window(context, document.get(), Vector2i(360, 190));
    window->center_horizontal();
    window->center_vertical();

    gui::Text_style text_style;
    text_style.font = font_library.font_by_name(gui::fonts::Sans);
    text_style.character_size = 16;
    text_style.color = sf::Color::White;

    auto header = window->create_child<gui::Text_element>("Select Random", text_style);
    header->center_horizontal();
    header->set_position({ 0.0, 25.0 });

    text_style.character_size = 15;

    gui::Vertical_list_style option_list_style;
    option_list_style.row_size = { 280.0, 25.0 };
    option_list_style.alternate_style = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 15));

    auto option_list = window->create_child<gui::Vertical_list<gui::Element>>(option_list_style);
    option_list->set_position({ 30.0, 60.0 });

    auto hover_style = text_style;

    {
        auto row = option_list->create_row();

        auto track_count = row->create_child<gui::Text_element>("Track number:", text_style);
        track_count->set_offset({ 10.0, 0.0 });

        gui::Integer_input_style input_style;
        input_style.arrow_area_size = { 20.0, 25.0 };
        input_style.text_area_size = { 30.0, 25.0 };
        input_style.text_style = text_style;
        input_style.text_hover_style = text_style;
        input_style.text_hover_style.color = sf::Color(255, 150, 0);

        gui::Integer_input_range input_range;
        input_range.min = 1;
        input_range.value = 1;
        input_range.max = 100;

        track_number_input_ = row->create_child<gui::Integer_input>(input_style, input_range);
        track_number_input_->set_position({ 200.0, 0.0 });
    }

    {
        auto row = option_list->create_row();

        auto allow_duplicates = row->create_child<gui::Text_element>("Allow duplicates:", text_style);
        allow_duplicates->set_offset({ 10.0, 0.0 });

        utf8_string yes = "Yes";
        utf8_string no = "No";

        allow_duplicates_text_ = row->create_child<gui::Text_element>(allow_duplicates_ ? yes : no, text_style);
        allow_duplicates_text_->center_text_horizontal();
        allow_duplicates_text_->set_size({ 80.0, 25.0 });
        allow_duplicates_text_->set_position({ 200.0, 0.0 });


        
        hover_style.color = sf::Color(255, 150, 0);
        allow_duplicates_text_->register_style(gui::states::hover, hover_style);

        allow_duplicates_text_->add_event_handler(gui::events::on_click, 
            [this, yes, no](const gui::Element& element)
        {
            allow_duplicates_ = !allow_duplicates_;
            allow_duplicates_text_->set_text(allow_duplicates_ ? yes : no);
        });
    }

    text_style.character_size = hover_style.character_size = 16;

    auto confirm_button = window->create_child<gui::Text_element>("OK!", text_style);
    confirm_button->set_position({ 100.0, 125.0 });
    confirm_button->set_size({ 80.0, 25.0 });
    confirm_button->center_text_horizontal();
    confirm_button->register_style(gui::states::hover, hover_style);

    auto cancel_button = window->create_child<gui::Text_element>("Cancel", text_style);
    cancel_button->set_position({ 180.0, 125.0 });
    cancel_button->set_size({ 80.0, 25.0 });
    cancel_button->center_text_horizontal();
    cancel_button->register_style(gui::states::hover, hover_style);

    auto confirm = [this]()
    {
        select_random_tracks();

        hide_random_selection_dialog();
    };

    auto cancel = [this]()
    {
        hide_random_selection_dialog();
    };

    confirm_button->add_event_handler(gui::events::on_click, 
        [confirm](const gui::Element& element)
    {
        confirm();
    });

    confirm_button->add_event_handler(gui::events::on_key_down, 
        [confirm](const gui::Element& element, sf::Keyboard::Key key, unsigned int key_modifiers)
    {
        if (key == sf::Keyboard::Return)
        {
            confirm();
        }
    });

    cancel_button->add_event_handler(gui::events::on_click,
        [cancel](const gui::Element& element)
    {
        cancel();
    });

    cancel_button->add_event_handler(gui::events::on_key_down,
        [cancel](const gui::Element& element, sf::Keyboard::Key key, unsigned int key_modifiers)
    {
        if (key == sf::Keyboard::Escape)
        {
            cancel();
        }
    });

    document->set_visible(false);

    return document;
}

ts::gui::Document_handle ts::states::Track_setup_menu::create_track_setup_document(Cup_setup_menu* cup_setup_menu)
{
    auto context = cup_setup_menu->context();

    auto document = context->create_document("track-setup");

    auto window = cup_setup_menu->create_containment_window(document.get(), "Track Setup");

    auto window_size = window->size();

    auto& font_library = context->font_library();

    auto font = font_library.font_by_name(gui::fonts::Sans);

    gui::Grid_list_style grid_style;
    grid_style.cell_size = Vector2<double>(130.0, 25.0);

    std::size_t num_columns = static_cast<std::size_t>((window_size.x - 90.0) / grid_style.cell_size.x);

    Vector2<double> track_scroll_pane_size(num_columns * grid_style.cell_size.x, (window_size.y - 160.0) * 0.5);

    auto track_scroll_pane = window->create_child<gui::Scroll_pane>(track_scroll_pane_size, gui::Scroll_pane_style());
    track_scroll_pane->set_position({ 40.0, 60.0 });

    auto scrollbar_style = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 25));

    track_scroll_pane->vertical_scroll_bar()->slider()->set_background_style(scrollbar_style);
    track_scroll_pane->vertical_scroll_bar()->set_background_style(scrollbar_style);

    track_grid_list_ = track_scroll_pane->create_child<track_grid_type>(grid_style, num_columns);

    gui::Text_style info_text_style;
    info_text_style.character_size = 14;
    info_text_style.color = sf::Color(200, 200, 200);
    info_text_style.font = font;

    selected_tracks_text_ = window->create_child<gui::Text_element>("Selected tracks:", info_text_style);
    selected_tracks_text_->set_position({ 40.0, track_scroll_pane_size.y + 85.0 });

    Vector2<double> selection_scroll_pane_size(180.0, track_scroll_pane_size.y);
    Vector2<double> selection_scroll_pane_position(40.0, track_scroll_pane_size.y + 110);


    selection_scroll_pane_ = window->create_child<gui::Scroll_pane>(selection_scroll_pane_size, gui::Scroll_pane_style());
    selection_scroll_pane_->set_position(selection_scroll_pane_position);

    selection_scroll_pane_->vertical_scroll_bar()->slider()->set_background_style(scrollbar_style);
    selection_scroll_pane_->vertical_scroll_bar()->set_background_style(scrollbar_style);

    gui::Vertical_list_style list_style;
    list_style.row_size = Vector2<double>(180.0, 25.0);
    list_style.alternate_style = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 15));

    track_selection_list_ = selection_scroll_pane_->create_child<selection_list_type>(list_style);

    selected_track_style_.character_size = 14;
    selected_track_style_.color = sf::Color::White;
    selected_track_style_.font = font;

    selected_track_hover_style_ = selected_track_style_;
    selected_track_hover_style_.color = sf::Color(255, 150, 0);

    Vector2<double> controls_container_size(window_size.x - 220.0, selection_scroll_pane_size.y);

    auto controls_container = window->create_child<gui::Element>(controls_container_size);
    controls_container->set_position({ 260.0, selection_scroll_pane_position.y });

    Vector2<double> control_size(120.0, 25.0);

    auto control_style = selected_track_style_;
    auto control_hover_style = selected_track_hover_style_;
    control_style.character_size = control_hover_style.character_size = 15;

    auto control_inactive_style = control_style;
    control_inactive_style.color = sf::Color(150, 150, 150);

    auto clear_all = controls_container->create_child<gui::Text_element>("Clear All", control_style);
    clear_all->set_position({ 0.0, 0.0 });
    clear_all->set_size(control_size);
    clear_all->register_style(gui::states::hover, control_hover_style);

    auto select_random = controls_container->create_child<gui::Text_element>("Select random...", control_style);
    select_random->set_position({ 0.0, 25.0 });
    select_random->set_size(control_size);
    select_random->register_style(gui::states::hover, control_hover_style);

    remove_selection_button_ = controls_container->create_child<gui::Text_element>("Remove", control_style);

    remove_selection_button_->set_position({ 0.0, 50.0 });
    remove_selection_button_->set_size(control_size);

    remove_selection_button_->register_style(gui::states::hover, control_hover_style);
    remove_selection_button_->register_style(gui::states::inactive, control_inactive_style);

    remove_selection_button_->set_active(false);

    move_up_button_ = controls_container->create_child<gui::Text_element>("Move up", control_style);
    move_up_button_->set_position({ 0.0, 75.0 });
    move_up_button_->set_size(control_size);

    move_up_button_->register_style(gui::states::hover, control_hover_style);
    move_up_button_->register_style(gui::states::inactive, control_inactive_style);

    move_up_button_->set_active(false);

    move_down_button_ = controls_container->create_child<gui::Text_element>("Move down", control_style);
    move_down_button_->set_position({ 0.0, 100.0 });
    move_down_button_->set_size(control_size);

    move_down_button_->register_style(gui::states::hover, control_hover_style);
    move_down_button_->register_style(gui::states::inactive, control_inactive_style);

    move_down_button_->set_active(false);

    clear_all->add_event_handler(gui::events::on_click, 
        [this](const gui::Element& element)
    {
        clear_selection();
    });

    select_random->add_event_handler(gui::events::on_click,
        [this](const gui::Element& element)
    {
        show_random_selection_dialog();
    });

    remove_selection_button_->add_event_handler(gui::events::on_key_down, 
        [this](const gui::Element& element, sf::Keyboard::Key key, unsigned int modifiers)
    {
        if (key == sf::Keyboard::Delete)
        {
            remove_selection();
        }
    });

    remove_selection_button_->add_event_handler(gui::events::on_click,
        [this](const gui::Element& element)
    {
        remove_selection();
    });

    move_up_button_->add_event_handler(gui::events::on_click,
        [this](const gui::Element& element)
    {
        move_selection_up();
    });

    move_down_button_->add_event_handler(gui::events::on_click,
        [this](const gui::Element& element)
    {
        move_selection_down();
    });

    move_up_button_->add_event_handler(gui::events::on_key_down,
        [this](const gui::Element& element, sf::Keyboard::Key key, unsigned int key_modifiers)
    {
        if (key == sf::Keyboard::Up)
        {
            move_selection_up();
        }
    });


    move_down_button_->add_event_handler(gui::events::on_key_down,
        [this](const gui::Element& element, sf::Keyboard::Key key, unsigned int key_modifiers)
    {
        if (key == sf::Keyboard::Down)
        {
            move_selection_down();
        }
    });

    auto& track_store = cup_setup_menu->resource_store()->track_store();
    open_directory(track_store.root_directory());

    populate_selected_tracks_list();

    document->set_visible(false);

    return document;
}

void ts::states::Track_setup_menu::remove_selection()
{
    if (selection_index_ != 0)
    {
        auto& track_settings = cup_setup_menu_->resource_store()->track_settings();

        auto index = selection_index_ - 1;

        track_selection_list_->delete_row(index);
        track_settings.remove_track(index);

        for (auto it = track_settings.selected_tracks().begin() + index; it != track_settings.selected_tracks().end(); ++it, ++index)
        {
            auto text = track_selection_list_->get_row(index);
            text->set_text(make_selected_track_string(*it, index + 1));
        }

        select_track(selection_index_);
    }

    cup_setup_menu_->check_startability();
}

void ts::states::Track_setup_menu::move_selection_up()
{
    if (selection_index_ > 1)
    {
        auto& track_settings = cup_setup_menu_->resource_store()->track_settings();
        const auto& selected_tracks = track_settings.selected_tracks();

        auto index = selection_index_ - 1;

        track_settings.move_forward(index);

        auto prev_row = track_selection_list_->get_row(index - 1);
        prev_row->set_text(make_selected_track_string(selected_tracks[index - 1], index));

        auto row = track_selection_list_->get_row(index);
        row->set_text(make_selected_track_string(selected_tracks[index], index + 1));

        select_track(index);
    }
}

void ts::states::Track_setup_menu::move_selection_down()
{
    auto& track_settings = cup_setup_menu_->resource_store()->track_settings();
    const auto& selected_tracks = track_settings.selected_tracks();

    if (selection_index_ < selected_tracks.size())
    {
        auto index = selection_index_ - 1;

        track_settings.move_backward(index);

        auto next_row = track_selection_list_->get_row(index + 1);
        next_row->set_text(make_selected_track_string(selected_tracks[index + 1], index + 2));

        auto row = track_selection_list_->get_row(index);
        row->set_text(make_selected_track_string(selected_tracks[index], index + 1));

        select_track(selection_index_ + 1);
    }
}

void ts::states::Track_setup_menu::clear_selection()
{
    auto& track_settings = cup_setup_menu_->resource_store()->track_settings();

    track_selection_list_->clear();

    track_settings.clear_selection();

    cup_setup_menu_->check_startability();
}

void ts::states::Track_setup_menu::populate_selected_tracks_list()
{
    auto& track_settings = cup_setup_menu_->resource_store()->track_settings();

    track_selection_list_->clear();

    for (auto& track : track_settings.selected_tracks())
    {
        append_selected_track(track);
    }
}

ts::utf8_string ts::states::Track_setup_menu::make_selected_track_string(resources::Track_handle track, std::size_t selection_index)
{
    utf8_string track_string = std::to_string(selection_index) + ". ";
    track_string += track.name();
    return track_string;
}

void ts::states::Track_setup_menu::append_selected_track(resources::Track_handle track)
{
    auto selection_index = track_selection_list_->row_count() + 1;

    auto string = make_selected_track_string(track, selection_index);

    auto text = track_selection_list_->create_row(std::move(string), selected_track_style_);

    text->set_offset({ 8.0, 0.0 });

    text->register_style(gui::states::hover, selected_track_hover_style_);
    text->register_style(gui::states::selected, selected_track_hover_style_);

    text->add_event_handler(gui::events::on_click, 
        [this, selection_index](const gui::Element& element)
    {
        select_track(selection_index);
    });
}

void ts::states::Track_setup_menu::select_track(std::size_t index)
{
    if (selection_index_ != 0)
    {
        auto text_elem = track_selection_list_->get_row(selection_index_ - 1);
        if (text_elem)
        {
            text_elem->set_state(gui::states::selected, false);
        }
    }

    if (index != 0)
    {
        auto text_elem = track_selection_list_->get_row(index - 1);

        if (text_elem)
        {
            text_elem->set_state(gui::states::selected, true);
        }

        else
        {
            index = 0;
        }
    }

    bool activate = index != 0;
    remove_selection_button_->set_active(activate);
    move_up_button_->set_active(activate);
    move_down_button_->set_active(activate);

    selection_index_ = index;
}

void ts::states::Track_setup_menu::add_track(resources::Track_handle track)
{
    auto& track_settings = cup_setup_menu_->resource_store()->track_settings();    

    track_settings.add_track(track);
    append_selected_track(track);

    auto scroll_bar = selection_scroll_pane_->vertical_scroll_bar();
    if (scroll_bar->visible())
    {
        // Scroll down, to show the recent addition
        scroll_bar->set_value(scroll_bar->range().end);
    }

    cup_setup_menu_->check_startability();
}

void ts::states::Track_setup_menu::open_directory(resources::Track_directory directory)
{
    auto context = cup_setup_menu_->context();

    auto& font_library = context->font_library();

    gui::Text_style cell_text_style;
    cell_text_style.character_size = 15;
    cell_text_style.color = sf::Color(200, 200, 200);
    cell_text_style.font = font_library.font_by_name(gui::fonts::Sans);

    auto cell_text_hover_style = cell_text_style;
    cell_text_hover_style.color = sf::Color(255, 150, 0);

    track_grid_list_->clear();

    if (auto parent_dir = directory.parent_directory())
    {
        auto cell = track_grid_list_->create_cell();
        auto text = cell->create_child<gui::Text_element>("../", cell_text_style);

        text->register_style(gui::states::hover, cell_text_hover_style);
        text->set_offset({ 8.0, 0.0 });

        text->set_size({ 1.0, 1.0 }, gui::relative);

        cell->add_event_handler(gui::events::on_click,
            [this, parent_dir](gui::Element& element)
        {
            open_directory(parent_dir);
        });
    }

    for (auto dir : directory.directories())
    {
        auto cell = track_grid_list_->create_cell();
        auto text = cell->create_child<gui::Text_element>(dir.dir_name() + "/", cell_text_style);

        text->register_style(gui::states::hover, cell_text_hover_style);
        text->set_offset({ 8.0, 0.0 });

        text->set_size({ 1.0, 1.0 }, gui::relative);

        cell->add_event_handler(gui::events::on_click,
            [this, dir](const gui::Element& element)
        {
            open_directory(dir);
        });
    }

    cell_text_style.color = sf::Color::White;

    for (auto track : directory.tracks())
    {
        auto cell = track_grid_list_->create_cell();
        auto text = cell->create_child<gui::Text_element>(track.name(), cell_text_style);

        text->register_style(gui::states::hover, cell_text_hover_style);
        text->set_offset({ 8.0, 0.0 });

        text->set_size({ 1.0, 1.0 }, gui::relative);

        cell->add_event_handler(gui::events::on_click,
            [this, track](const gui::Element& element)
        {
            add_track(track);
        });
    }

    current_directory_ = directory;
}

void ts::states::Track_setup_menu::select_random_tracks()
{
    auto& track_store = cup_setup_menu_->resource_store()->track_store();
    auto& track_settings = cup_setup_menu_->resource_store()->track_settings();
    const auto& selected_tracks = track_settings.selected_tracks();

    auto random_count = track_number_input_->value();

    auto prior_size = selected_tracks.size();

    if (allow_duplicates_)
    {
        for (auto n = 0; n != random_count; ++n)
        {
            track_settings.add_track(track_store.random_track(current_directory_));
        }
    }
    
    else
    {
        std::set<resources::Track_handle> disallowed(selected_tracks.begin(), selected_tracks.end());
        std::vector<resources::Track_handle> random_selection;

        track_store.select_random_tracks(current_directory_, random_count, std::back_inserter(random_selection), disallowed);

        for (auto&& track : random_selection)
        {
            track_settings.add_track(std::move(track));
        }
    }

    for (auto it = selected_tracks.begin() + prior_size; it != selected_tracks.end(); ++it)
    {
        append_selected_track(*it);
    }

    cup_setup_menu_->check_startability();
}

void ts::states::Track_setup_menu::show_random_selection_dialog()
{
    track_setup_document_->set_active(false);

    random_selection_dialog_->set_visible(true);
}

void ts::states::Track_setup_menu::hide_random_selection_dialog()
{
    track_setup_document_->set_active(true);

    random_selection_dialog_->set_visible(false);
}

void ts::states::Track_setup_menu::show()
{
    track_setup_document_->set_visible(true);
}

void ts::states::Track_setup_menu::hide()
{
    track_setup_document_->set_visible(false);
}

ts::states::Car_setup_menu::Car_setup_menu(Cup_setup_menu* cup_setup_menu)
: car_settings_(&cup_setup_menu->resource_store()->car_settings()),
  cup_setup_menu_(cup_setup_menu),
  car_setup_document_(create_car_setup_document(cup_setup_menu))
{
}

ts::gui::Document_handle ts::states::Car_setup_menu::create_car_setup_document(Cup_setup_menu* cup_setup_menu)
{
    auto context = cup_setup_menu->context();
    auto document = context->create_document("car-setup");

    auto window = cup_setup_menu->create_containment_window(document.get(), "Car Setup");
    auto window_size = window->size();

    auto& font_library = context->font_library();
    auto font = font_library.font_by_name(gui::fonts::Sans);

    Vector2<double> scroll_pane_size(320.0, window_size.y - 140.0);
    auto scroll_pane = window->create_child<gui::Scroll_pane>(scroll_pane_size, gui::Scroll_pane_style());

    scroll_pane->set_position({ 50.0, 80.0 });

    gui::Vertical_list_style car_list_style;
    car_list_style.row_size = { 320.0, 40.0 };
    car_list_style.item_style = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 15));
    car_list_style.alternate_style = car_list_style.item_style;
    car_list_style.row_spacing = 4.0;

    auto car_list_selected_style = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 40));

    auto car_list = scroll_pane->create_child<gui::Vertical_list<gui::Element>>(car_list_style);

    gui::Text_style car_text_style;
    car_text_style.font = font;
    car_text_style.character_size = 16;
    car_text_style.color = sf::Color::White;

    auto car_text_hover_style = car_text_style;
    car_text_hover_style.color = sf::Color(255, 150, 0);

    auto& car_store = cup_setup_menu->resource_store()->car_store();
    for (auto car_handle : car_store)
    {
        resources::Car_identifier car_identifier;
        car_identifier.car_name = car_handle->car_name;

        auto row = car_list->create_row();

        auto text = row->create_child<gui::Text_element>(car_handle->car_name, car_text_style);
        text->set_size({ 1.0, 1.0 }, gui::relative);
        text->set_offset({ 12.0, 8.0 });

        text->register_style(gui::states::hover, car_text_hover_style);

        row->register_background_style(gui::states::selected, car_list_selected_style);

        row->add_event_handler(gui::events::on_click,
            [this, car_identifier](gui::Element& row)
        {
            toggle_car_selection(car_identifier);
        });

        car_rows_.push_back({ car_identifier, row });

        auto car_img = row->create_child<gui::Element>(Vector2<double>(40.0, 40.0));
        car_img->set_position({ 280.0, 0.0 });
        
        auto texture = context->load_texture(car_handle->image_file);

        auto texture_rect = car_handle->image_rect;
        if (car_handle->image_type == resources::Image_type::Rotational)
        {
            texture_rect.width = texture_rect.height;
        }

        auto texture_mode = gui::Textured_background::Mode::Centered;

        auto textured_background = gui::make_background_style<gui::Textured_background>(texture, texture_rect, texture_mode);

        auto scale = 2.0 / car_handle->image_scale;

        textured_background.background->set_scale({ scale, scale });

        car_img->set_background_style(textured_background);
    }

    gui::Vertical_list_style options_list_style;
    options_list_style.item_style = car_list_style.item_style;
    options_list_style.row_size = { 360.0, 25.0 };

    auto settings_string_style = car_text_style;
    settings_string_style.color = sf::Color(150, 150, 150);

    auto settings_text_style = car_text_style;
    auto settings_text_hover_style = car_text_style;
    settings_text_hover_style.color = sf::Color(255, 150, 0);

    auto settings_string = window->create_child<gui::Text_element>("Settings", settings_string_style);
    settings_string->set_position({ 80.0, window_size.y - 110.0 });

    auto options_list = window->create_child<gui::Vertical_list<gui::Element>>(options_list_style);
    options_list->set_position({ 60.0, window_size.y - 80.0 });

    {
        auto row = options_list->create_row();

        auto car_mode_setting = row->create_child<gui::Text_element>("Car mode:", settings_text_style);

        car_mode_setting->set_offset({ 10.0, 0.0 });

        auto string_value = car_mode_to_string(car_settings_->car_mode());

        car_mode_setting_string_ = row->create_child<gui::Text_element>(string_value, settings_text_style);
        car_mode_setting_string_->set_size({ 160.0, 25.0 });
        car_mode_setting_string_->set_position({ 160.0, 0.0 });
        car_mode_setting_string_->register_style(gui::states::hover, settings_text_hover_style);
        car_mode_setting_string_->center_text_horizontal();

        car_mode_setting_string_->add_event_handler(gui::events::on_click, 
            [this](const gui::Element& element)
        {
            traverse_car_mode();
        });
    }

    update_selection_rows();

    document->set_visible(false);

    return document;
}

ts::utf8_string ts::states::Car_setup_menu::car_mode_to_string(resources::Car_mode car_mode) const
{
    switch (car_mode)
    {
    case resources::Car_mode::Free:
        return "Free";
    case resources::Car_mode::Fixed:
        return "Fixed";
    case resources::Car_mode::Random:
        return "Randomly Fixed";
    }

    return "";
}

void ts::states::Car_setup_menu::traverse_car_mode()
{
    auto new_car_mode = [this]()
    {
        switch (car_settings_->car_mode())
        {
        case resources::Car_mode::Free:
            return resources::Car_mode::Fixed;
        case resources::Car_mode::Fixed:
            return resources::Car_mode::Random;        

        default:
            return resources::Car_mode::Free;
        }
    }();

    car_settings_->set_car_mode(new_car_mode);

    car_mode_setting_string_->set_text(car_mode_to_string(new_car_mode));

    update_selection_rows();
}

void ts::states::Car_setup_menu::toggle_car_selection(const resources::Car_identifier& car_identifier)
{
    if (car_settings_->is_car_selected(car_identifier))
    {
        car_settings_->deselect_car(car_identifier);
    }

    else
    {
        car_settings_->select_car(car_identifier);
    }

    update_selection_rows();
    cup_setup_menu_->check_startability();
}

void ts::states::Car_setup_menu::update_selection_rows()
{
    for (auto& row : car_rows_)
    {
        bool selected = car_settings_->is_car_selected(row.car_identifier);

        row.row_elem->set_state(gui::states::selected, selected);
    }
}


void ts::states::Car_setup_menu::show()
{
    car_setup_document_->set_visible(true);
}

void ts::states::Car_setup_menu::hide()
{
    car_setup_document_->set_visible(false);
}