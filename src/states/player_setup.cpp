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
#include "player_setup.hpp"

#include "gui_definitions/window_template.hpp"


ts::states::Player_setup_menu::Player_setup_menu(Main_menu* main_menu)
: main_menu_(main_menu),
player_selection_menu_(std::make_unique<Player_selection_menu>(this)),
color_selection_menu_(std::make_unique<Color_selection_menu>(this)),
player_setup_document_(create_player_setup_document())
{
}

void ts::states::Player_setup_menu::return_to_main_menu()
{
    player_setup_document_->set_visible(false);

    player_selection_menu_->hide();

    main_menu_->show();
}

void ts::states::Player_setup_menu::hide()
{
    player_setup_document_->set_visible(false);
    player_selection_menu_->hide();
    color_selection_menu_->hide();
}

void ts::states::Player_setup_menu::show()
{
    player_setup_document_->set_active(true);
    player_setup_document_->set_visible(true);

    player_selection_menu_->hide();
    color_selection_menu_->hide();
}

ts::gui::Context* ts::states::Player_setup_menu::context() const
{
    return main_menu_->context();
}

ts::resources::Resource_store* ts::states::Player_setup_menu::resource_store() const
{
    return main_menu_->resource_store();
}


void ts::states::Player_setup_menu::show_color_selection_dialog(resources::Player_handle player_handle)
{
    return_to_player_selection_ = !player_setup_document_->visible();

    player_setup_document_->set_visible(false);
    player_selection_menu_->hide();

    color_selection_menu_->show(player_handle);
}

void ts::states::Player_setup_menu::return_from_color_selection(resources::Player_handle player, resources::Player_color new_color)
{
    if (return_to_player_selection_)
    {
        player_selection_menu_->show();
    }

    else
    {
        player_selection_menu_->hide();
        player_setup_document_->set_visible(true);
    }

    resource_store()->players.set_player_color(player, new_color);

    player_selection_menu_->update_color_preview(player, new_color);

    update_selection();
}

void ts::states::Player_setup_menu::update_selection()
{
    auto& player_store = main_menu_->resource_store()->players;
    auto& player_settings = main_menu_->resource_store()->settings.player_settings;

    for (auto slot = 0; slot != 4; ++slot)
    {
        auto unique_id = player_settings.selected_players[slot];
        auto text_elem = slots_[slot].name_text;
        auto color_elem = slots_[slot].color_element;
        auto& color_background = slots_[slot].color_background;

        auto player_handle = player_store.get_player_by_id(unique_id);
        
        if (player_handle)
        {
            text_elem->set_text(player_handle->name);
            color_background->set_color(player_handle->color);
            color_elem->set_visible(true);
        }

        else
        {
            color_elem->set_visible(false);
            text_elem->set_text({});
        }
    }
}

void ts::states::Player_setup_menu::goto_player_selection(int slot)
{
    player_setup_document_->set_visible(false);

    player_selection_menu_->show(slot);
}


ts::gui::Document_handle ts::states::Player_setup_menu::create_player_setup_document()
{
    auto context = main_menu_->context();
    auto& font_library = context->font_library();
    auto screen_size = context->screen_size();

    auto font = font_library.font_by_name(gui::fonts::Sans);

    auto document = context->create_document("player-setup");

    Vector2i container_size(600, 440);
    auto container = gui_definitions::create_styled_window(context, document.get(), container_size);

    container->center_horizontal();
    container->center_vertical();

    gui::Text_style header_style;
    header_style.font = font;
    header_style.character_size = 16;
    header_style.color = sf::Color::White;

    auto slot_text_style = header_style;
    slot_text_style.character_size = 15;

    auto header = container->create_child<gui::Text_element>("Player Setup", header_style);
    header->set_position({ 0.0, 25.0 });
    header->center_horizontal();


    Vector2<double> slot_elem_size(400.0, 32.0);

    gui::Vertical_list_style slot_list_style;
    slot_list_style.row_size = { 300.0, 32.0 };
    slot_list_style.row_spacing = 4.0;

    using slot_list_type = gui::Vertical_list<gui::Element>;

    auto slot_list = container->create_child<slot_list_type>(slot_list_style);
    slot_list->set_position({ 40.0, 60.0 });

    auto slot_elem_style = gui::make_background_style<gui::Plain_background>(sf::Color(100, 100, 100, 35));
    auto slot_elem_hover_style = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 35));

    const auto& player_store = main_menu_->resource_store()->players;
    const auto& player_settings = main_menu_->resource_store()->settings.player_settings;
    const auto& selected_players = player_settings.selected_players;

    using namespace gui::events;

    for (auto slot = 0; slot != 4; ++slot)
    {
        auto selected_player = selected_players[slot];
        auto player_data = player_store.get_player_by_id(selected_player);

        auto slot_elem = slot_list->create_row();
        auto id_string = "Player " + std::to_string(slot + 1) + ":";

        slot_elem->set_background_style(slot_elem_style);
        slot_elem->register_background_style(gui::states::hover, slot_elem_hover_style);
        slot_elem->register_background_style(gui::states::selected, slot_elem_hover_style);

        auto id_text = slot_elem->create_child<gui::Text_element>(id_string, slot_text_style);
        id_text->set_offset({ 5.0, 5.0 });


        auto name_text = slot_elem->create_child<gui::Text_element>(player_data ? player_data->name : "", slot_text_style);
        name_text->set_offset({ 120.0, 5.0 });

        auto color_elem = slot_elem->create_child<gui::Element>(Vector2i(28, 28));
        auto color_background = gui::make_background_style<gui::Player_color_background>();
        color_elem->set_background_style(color_background);
        color_elem->set_position({ 270.0, 2.0 });

        if (player_data)
        {
            color_background.background->set_color(player_data->color);
        }

        else
        {
            color_elem->set_visible(false);
        }

        slot_elem->add_event_handler(on_click, [slot, this](gui::Element&) { goto_player_selection(slot); });

        slots_[slot].slot_id = slot;
        slots_[slot].name_text = name_text;
        slots_[slot].color_background = color_background.background;
        slots_[slot].color_element = color_elem;
    }

    auto back_style = header_style;
    auto back_hover_style = back_style;
    back_hover_style.color = sf::Color(255, 150, 0);

    auto back = container->create_child<gui::Text_element>("Back", back_style);
    back->set_size({ 60.0, 25.0 });
    back->set_position({ 40.0, container_size.y - 80.0 });
    back->set_offset({ 8.0, 2.0 });
    back->register_style(gui::states::hover, back_hover_style);

    back->add_event_handler(gui::events::on_click, 
        [this](const gui::Element& element)
    {
        return_to_main_menu();
    });

    back->add_event_handler(gui::events::on_key_down, 
        [this](const gui::Element& element, sf::Keyboard::Key key, unsigned int modifiers)
    {
        if (key == sf::Keyboard::Escape)
        {
            return_to_main_menu();
        }
    });

    document->set_visible(false);

    return document;
}

ts::gui::Document_handle ts::states::Player_selection_menu::create_player_selection_document()
{
    auto context = player_setup_menu_->context();
    auto& font_library = context->font_library();
    auto screen_size = context->screen_size();

    const auto& player_store = player_setup_menu_->resource_store()->players;

    auto font = font_library.font_by_name(gui::fonts::Sans);

    auto document = context->create_document("player-selection");

    Vector2i container_size(600, std::max(440, screen_size.y - 240));

    auto container = gui_definitions::create_styled_window(context, document.get(), container_size);
    container->center_horizontal();
    container->center_vertical();

    auto scroll_pane_height = container_size.y - 200.0;

    gui::Scroll_pane_style scroll_pane_style;
    scroll_pane_ = container->create_child<gui::Scroll_pane>(Vector2<double>(200.0, scroll_pane_height), scroll_pane_style);
    scroll_pane_->set_position({ 40.0, 60.0 });

    auto background_style = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 25));

    scroll_pane_->vertical_scroll_bar()->set_background_style(background_style);
    scroll_pane_->vertical_scroll_bar()->slider()->set_background_style(background_style);

    gui::Vertical_list_style list_style;
    list_style.row_size = { 200.0, 24.0 };
    list_style.row_spacing = 0.0;
    list_style.item_style = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 10));

    list_text_style_.font = font;
    list_text_style_.character_size = 15;
    list_text_style_.color = sf::Color::White;

    list_text_hover_style_ = list_text_style_;
    list_text_hover_style_.color = sf::Color(255, 150, 0);

    auto header_style = list_text_style_;
    header_style.character_size = 17;

    auto header_text = container->create_child<gui::Text_element>("Player Selection", list_text_style_);
    header_text->set_position({ 0.0, 25.0 });
    header_text->center_horizontal();

    player_list_ = scroll_pane_->create_child<player_list_type>(list_style);

    for (auto player_handle : player_store)
    {
        create_player_list_row(player_handle);
    }

    create_player_list_row(resources::Player_handle());

    auto button_style = list_text_style_;
    auto button_hover_style = list_text_hover_style_;
    button_hover_style.character_size = button_style.character_size = 16;

    auto create_player = container->create_child<gui::Text_element>("Create new...", button_style);
    create_player->set_position({ 40.0, container_size.y - 120.0 });
    create_player->set_size({ 180.0, 30.0 });
    create_player->register_style(gui::states::hover, button_hover_style);

    auto editbox_style = list_text_hover_style_;

    create_player->add_event_handler(gui::events::on_click, [this, editbox_style](gui::Element& element)
    {
        deselect_all();

        if (!player_creation_dialog_->visible())
        {
            show_player_creation_dialog();
        }
    });

    auto delete_player = container->create_child<gui::Text_element>("Delete...", button_style);
    delete_player->set_position({ 40.0, container_size.y - 90.0 });
    delete_player->set_size({ 180.0, 30.0 });
    delete_player->register_style(gui::states::hover, button_hover_style);

    delete_player->add_event_handler(gui::events::on_click, [this](gui::Element& element)
    {
        show_player_deletion_dialog();
    });

    document->add_event_handler(gui::events::on_key_down, [this](gui::Element& element, sf::Keyboard::Key key, unsigned int modifiers)
    {
        if (key == sf::Keyboard::Delete)
        {
            show_player_deletion_dialog();
        }
    });

    auto back = container->create_child<gui::Text_element>("Back", button_style);
    back->set_position({ 40.0, container_size.y - 60.0 });
    back->set_size({ 180.0, 30.0 });

    back->register_style(gui::states::hover, button_hover_style);

    back->add_event_handler(gui::events::on_click, [this](gui::Element& element)
    {
        return_to_player_setup();
    });

    return document;
}


ts::gui::Document_handle ts::states::Player_selection_menu::create_player_creation_dialog()
{
    auto context = player_setup_menu_->context();
    auto& font_library = context->font_library();

    auto font = font_library.font_by_name(gui::fonts::Sans);

    auto document = context->create_document("player-creation");

    auto container = gui_definitions::create_styled_window(context, document.get(), Vector2i(440, 160));

    container->center_horizontal();
    container->center_vertical();

    gui::Text_style text_style;
    text_style.font = font;
    text_style.character_size = 15;
    text_style.color = sf::Color::White;

    auto enter_name = container->create_child<gui::Text_element>("Enter player name:", text_style);
    enter_name->set_position({ 20.0, 25.0 });
    enter_name->center_horizontal();

    auto background = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 20));

    auto editbox_area = container->create_child<gui::Element>(Vector2i(240, 25));
    editbox_area->set_position({ 20.0, 55.0 });
    editbox_area->set_background_style(background);
    editbox_area->center_horizontal();

    player_editbox_ = editbox_area->create_child<gui::Edit_box>(text_style);
    player_editbox_->center_horizontal();
    player_editbox_->set_max_length(resources::max_player_name_length);

    auto confirmation_button_area = container->create_child<gui::Element>(Vector2i(60, 25));
    confirmation_button_area->set_position({ 140.0, 90.0 });

    auto confirmation_button = confirmation_button_area->create_child<gui::Text_element>("OK!", text_style);
    confirmation_button->center_horizontal();

    auto cancel_button_area = container->create_child<gui::Element>(Vector2i(60, 25));
    cancel_button_area->set_position({ 240.0, 90.0 });

    auto cancel_button = cancel_button_area->create_child<gui::Text_element>("Cancel", text_style);
    cancel_button->center_horizontal();

    auto hover_style = text_style;
    hover_style.color = sf::Color(255, 150, 0, 255);

    confirmation_button->register_style(gui::states::hover, hover_style);
    cancel_button->register_style(gui::states::hover, hover_style);

    auto confirm_player_creation = [this]()
    {
        remove_none_row();
        create_player(player_editbox_->text());

        hide_player_creation_dialog();
        create_player_list_row(resources::Player_handle());

        scroll_down();        
    };

    confirmation_button->add_event_handler(gui::events::on_click,
        [this, confirm_player_creation](gui::Element& element)
    {
        confirm_player_creation();
    });

    confirmation_button->add_event_handler(gui::events::on_key_down,
        [this, confirm_player_creation](gui::Element& element, sf::Keyboard::Key key, unsigned int key_modifiers)
    {
        if (key == sf::Keyboard::Return)
        {
            confirm_player_creation();
        }
    });

    cancel_button->add_event_handler(gui::events::on_click,
        [this](gui::Element& elemnt)
    {
        hide_player_creation_dialog();
    });


    cancel_button->add_event_handler(gui::events::on_key_down,
        [this](gui::Element& element, sf::Keyboard::Key key, unsigned int modifiers)
    {
        if (key == sf::Keyboard::Escape)
        {
            hide_player_creation_dialog();
        }
    });

    return document;
}

ts::gui::Document_handle ts::states::Player_selection_menu::create_player_deletion_dialog()
{
    auto context = player_setup_menu_->context();
    auto& font_library = context->font_library();

    auto font = font_library.font_by_name(gui::fonts::Sans);

    auto document = context->create_document("player-deletion");

    auto container = gui_definitions::create_styled_window(context, document.get(), Vector2i(440, 120));
    container->center_horizontal();
    container->center_vertical();

    gui::Text_style text_style;
    text_style.font = font;
    text_style.character_size = 15;
    text_style.color = sf::Color::White;

    player_deletion_text_ = container->create_child<gui::Text_element>("", text_style);
    player_deletion_text_->center_horizontal();
    player_deletion_text_->set_position({ 0.0, 25.0 });

    auto confirmation_button_area = container->create_child<gui::Element>(Vector2i(60, 25));
    confirmation_button_area->set_position({ 140.0, 50.0 });

    auto confirmation_button = confirmation_button_area->create_child<gui::Text_element>("Yes", text_style);
    confirmation_button->center_horizontal();

    auto cancel_button_area = container->create_child<gui::Element>(Vector2i(60, 25));
    cancel_button_area->set_position({ 240.0, 50.0 });

    auto cancel_button = cancel_button_area->create_child<gui::Text_element>("No", text_style);
    cancel_button->center_horizontal();

    auto hover_style = text_style;
    hover_style.color = sf::Color(255, 150, 0, 255);

    confirmation_button->register_style(gui::states::hover, hover_style);
    cancel_button->register_style(gui::states::hover, hover_style);

    auto confirm = [this]()
    {
        delete_player();

        hide_player_deletion_dialog();
    };

    auto cancel = [this]()
    {
        hide_player_deletion_dialog();
    };

    confirmation_button->add_event_handler(gui::events::on_click,
        [confirm](gui::Element& element)
    {
        confirm();
    });

    confirmation_button->add_event_handler(gui::events::on_key_down,
        [confirm](gui::Element& element, sf::Keyboard::Key key, unsigned modifiers)
    {
        if (key == sf::Keyboard::Y || key == sf::Keyboard::Return)
        {
            confirm();
        }
    });

    cancel_button->add_event_handler(gui::events::on_click,
        [cancel](gui::Element& element)
    {
        cancel();
    });

    cancel_button->add_event_handler(gui::events::on_key_down,
        [cancel](gui::Element& element, sf::Keyboard::Key key, unsigned modifiers)
    {
        if (key == sf::Keyboard::N || key == sf::Keyboard::Escape)
        {
            cancel();
        }
    });

    return document;
}


void ts::states::Player_selection_menu::register_selection_handler(gui::Element* player_name_elem, resources::Player_handle player_handle)
{
    player_name_elem->add_event_handler(gui::events::on_click, [this, player_handle, player_name_elem](gui::Element& element)
    {
        bool selected = element.is_state_set(gui::states::selected);

        deselect_all();

        element.set_state(gui::states::selected, !selected);

        if (selected)
        {
            select_player(player_handle);
        }

        else
        {
            selected_player_ = player_handle;
            selected_player_row_ = player_name_elem->parent();
        }
    });
}

ts::states::Player_selection_menu::Player_selection_menu(Player_setup_menu* player_setup_menu)
: player_setup_menu_(player_setup_menu),
player_selection_document_(create_player_selection_document()),
player_deletion_dialog_(create_player_deletion_dialog()),
player_creation_dialog_(create_player_creation_dialog())
{
    player_selection_document_->set_visible(false);
    player_deletion_dialog_->set_visible(false);
    player_creation_dialog_->set_visible(false);
}

void ts::states::Player_selection_menu::show()
{
    player_selection_document_->set_active(true);
    player_selection_document_->set_visible(true);

    player_deletion_dialog_->set_visible(false);
    player_creation_dialog_->set_visible(false);
}

void ts::states::Player_selection_menu::show(int slot)
{
    current_slot_ = slot;

    player_selection_document_->set_active(true);
    player_selection_document_->set_visible(true);

    player_deletion_dialog_->set_visible(false);
    player_creation_dialog_->set_visible(false);
}

void ts::states::Player_selection_menu::hide()
{
    player_selection_document_->set_visible(false);
    player_deletion_dialog_->set_visible(false);
    player_creation_dialog_->set_visible(false);
}

void ts::states::Player_selection_menu::activate()
{
    player_selection_document_->set_active(true);
}

void ts::states::Player_selection_menu::deactivate()
{
    player_selection_document_->set_active(false);
}

void ts::states::Player_selection_menu::update_color_preview(resources::Player_handle player_handle, resources::Player_color color)
{
    auto range = color_preview_map_.equal_range(player_handle);
    for (auto it = range.first; it != range.second; ++it)
    {
        it->second->set_color(color);
    }
}

ts::resources::Player_handle ts::states::Player_selection_menu::create_player(const utf8_string& player_name)
{
    auto& player_store = player_setup_menu_->resource_store()->players;

    auto player_handle = player_store.create_player(player_name);

    create_player_list_row(player_handle);
    return player_handle;
}


void ts::states::Player_selection_menu::hide_player_deletion_dialog()
{
    player_selection_document_->set_active(true);

    player_deletion_dialog_->set_visible(false);
}

void ts::states::Player_selection_menu::delete_player()
{
    if (selected_player_)
    {
        auto& player_store = player_setup_menu_->resource_store()->players;

        player_store.delete_player(selected_player_);
        selected_player_ = resources::Player_handle();

        player_list_->delete_row(selected_player_row_);
        selected_player_row_ = nullptr;
    }
}

void ts::states::Player_selection_menu::remove_none_row()
{
    auto row_index = player_list_->row_count() - 1;
    auto row = player_list_->get_row(row_index);

    player_list_->delete_row(row_index);

    if (selected_player_row_ == row)
    {
        selected_player_row_ = nullptr;
    }
}

void ts::states::Player_selection_menu::create_player_list_row(resources::Player_handle player_handle)
{
    auto row = player_list_->create_row();
    auto row_size = row->size();

    const utf8_string& player_name = player_handle ? player_handle->name : "<none>";
    auto name_elem_size = row_size;
    name_elem_size.x -= 24.0;


    auto player_name_elem = row->create_child<gui::Text_element>(player_name, list_text_style_);
    player_name_elem->set_size(name_elem_size);

    player_name_elem->register_style(gui::states::hover, list_text_hover_style_);

    if (player_handle)
    {
        auto color_elem = row->create_child<gui::Element>(Vector2i(20, 20));
        color_elem->set_position({ name_elem_size.x + 2.0, 2.0 });
        
        auto color_hover_elem = color_elem->create_child<gui::Element>();
        color_hover_elem->set_size({ 1.0, 1.0 }, gui::relative);

        auto background = gui::make_background_style<gui::Player_color_background>(player_handle->color);
        color_elem->set_background_style(background);

        auto hover_background = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 50));
        color_hover_elem->register_background_style(gui::states::hover, hover_background);

        color_elem->add_event_handler(gui::events::on_click, 
            [this, player_handle](const gui::Element& element)
        {
            deactivate();

            player_setup_menu_->show_color_selection_dialog(player_handle);
        });

        color_preview_map_.insert(std::make_pair(player_handle, background.background));
    }

    register_selection_handler(player_name_elem, player_handle);
}

void ts::states::Player_selection_menu::scroll_down()
{
    auto scroll_bar = scroll_pane_->vertical_scroll_bar();

    if (scroll_bar->visible())
    {
        scroll_bar->set_value(scroll_bar->range().end);
    }
}

void ts::states::Player_selection_menu::show_player_deletion_dialog()
{
    if (selected_player_)
    {
        player_deletion_dialog_->set_active(true);
        player_deletion_dialog_->set_visible(true);

        player_selection_document_->set_active(false);

        player_deletion_text_->set_text(utf8_string("Delete player ") + selected_player_->name + "?");
    }
}

void ts::states::Player_selection_menu::show_player_creation_dialog()
{
    player_selection_document_->set_active(false);

    player_creation_dialog_->set_visible(true);

    player_editbox_->reset();
}

void ts::states::Player_selection_menu::hide_player_creation_dialog()
{
    player_selection_document_->set_active(true);

    player_creation_dialog_->set_visible(false);
}

void ts::states::Player_selection_menu::return_to_player_setup()
{
    deselect_all();

    player_selection_document_->set_visible(false);

    player_setup_menu_->show();
}

void ts::states::Player_selection_menu::deselect_all()
{
    const auto& rows = player_list_->children();
    for (auto& rows : rows)
    {
        auto& sibling = rows->children().front();
        sibling->set_state(gui::states::selected, false);
    }
}

void ts::states::Player_selection_menu::select_player(resources::Player_handle player_handle)
{
    auto& player_settings = player_setup_menu_->resource_store()->settings.player_settings;
    auto& selected_players = player_settings.selected_players;

    auto internal_id = player_handle ? player_handle->internal_id : 0;

    using unique_id = resources::Player_store::unique_id;

    std::transform(selected_players.begin(), selected_players.end(), selected_players.begin(),
        [internal_id](unique_id id) -> unique_id
    {
        if (id == internal_id) return 0;

        return id;
    });
    
    player_settings.selected_players[current_slot_] = internal_id;

    player_setup_menu_->update_selection();

    return_to_player_setup();
}

ts::states::Color_selection_menu::Color_selection_menu(Player_setup_menu* player_setup_menu)
: player_setup_menu_(player_setup_menu),
  color_dialog_(create_color_dialog())
{
}

void ts::states::Color_selection_menu::show(resources::Player_handle player_handle)
{
    color_dialog_->set_visible(true);

    player_handle_ = player_handle;

    player_name_text_->set_text(player_handle->name);

    set_color(player_handle->color);    

    select_base_component();
}

void ts::states::Color_selection_menu::set_color(resources::Player_color color)
{
    color_ = player_handle_->color;

    update_component_types();

    update_color_preview();
}

void ts::states::Color_selection_menu::hide()
{
    color_dialog_->set_visible(false);
}

void ts::states::Color_selection_menu::return_to_previous()
{
    hide();

    player_setup_menu_->return_from_color_selection(player_handle_, color_);
}

ts::gui::Document_handle ts::states::Color_selection_menu::create_color_dialog()
{
    auto context = player_setup_menu_->context();

    const auto& font_library = context->font_library();

    auto font = font_library.font_by_name(gui::fonts::Sans);

    auto document = context->create_document("color-dialog");

    auto container = gui_definitions::create_styled_window(context, document.get(), Vector2i(600, 440));
    container->center_horizontal();
    container->center_vertical();

    gui::Text_style name_text_style;
    name_text_style.font = font;
    name_text_style.color = sf::Color(255, 255, 255);
    name_text_style.character_size = 18;

    auto header_text_style = name_text_style;
    header_text_style.character_size = 16;

    auto color_setup = container->create_child<gui::Text_element>("Color Setup", header_text_style);
    color_setup->set_position({ 180.0, 30.0 });

    auto player_header = container->create_child<gui::Element>();
    player_header->set_size({ 240.0, 40.0 });
    player_header->set_position({ 40.0, 60.0 });

    auto back_button_style = header_text_style;
    auto back_button_hover_style = back_button_style;
    back_button_hover_style.color = sf::Color(255, 150, 0);

    auto back = container->create_child<gui::Text_element>("Back", header_text_style);
    back->register_style(gui::states::hover, back_button_hover_style);
    back->set_position({ 130.0, 360.0 });
    back->set_size({ 60, 25.0 });
    back->center_text_horizontal();

    auto subwindow_bg = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 10));

    auto player_color_style = gui::make_background_style<gui::Player_color_background>(resources::Player_color());

    player_color_background_ = player_color_style.background;

    auto player_color_preview = player_header->create_child<gui::Element>(Vector2i(40, 40));
    player_color_preview->set_position({ 200.0, 0.0 });

    player_color_preview->set_background_style(player_color_style);

    auto component_text_style = name_text_style;
    component_text_style.character_size = 14;
    component_text_style.color = sf::Color::White;

    auto component_text_hover_style = component_text_style;
    component_text_hover_style.color = sf::Color(255, 150, 0);

    player_name_text_ = player_header->create_child<gui::Text_element>("", name_text_style);
    player_name_text_->set_position({ 20.0, 10.0 });

    auto base_component_container = container->create_child<gui::Element>(Vector2i(240, 60));
    base_component_container->set_background_style(subwindow_bg);
    base_component_container->set_position({ 40.0, 120.0 });

    auto secondary_component_container = container->create_child<gui::Element>(Vector2i(240, 60));
    secondary_component_container->set_background_style(subwindow_bg);
    secondary_component_container->set_position({ 40.0, 200.0 });

    auto tertiary_component_container = container->create_child<gui::Element>(Vector2i(240, 60));
    tertiary_component_container->set_background_style(subwindow_bg);
    tertiary_component_container->set_position({ 40.0, 280.0 });

    Vector2i component_text_size(240, 25);

    gui::Option_set_style option_set_style;
    option_set_style.arrow_area_size = { 30.0, 25.0 };
    option_set_style.text_area_size = { 160.0, 25.0 };

    option_set_style.text_style = component_text_style;
    option_set_style.text_style.character_size = 14;

    option_set_style.text_hover_style = option_set_style.text_style;
    option_set_style.text_hover_style.color = sf::Color(255, 150, 0);

    using base_option_set = gui::Option_set<resources::Color_base::Type>;
    using extra_option_set = gui::Option_set<resources::Color_extra::Type>;

    using resources::Color_base;

    base_component_text_ = base_component_container->create_child<gui::Text_element>("Base component", component_text_style);
    base_component_text_->set_offset({ 0.0, 4.0 });
    base_component_text_->set_size(component_text_size);
    base_component_text_->center_text_horizontal();
    base_component_text_->register_style(gui::states::selected, component_text_hover_style);
    base_component_text_->register_style(gui::states::hover, component_text_hover_style);

    base_component_option_set_ = base_component_container->create_child<base_option_set>(option_set_style);
    base_component_option_set_->set_position({ 0.0, 30.0 });
    base_component_option_set_->center_horizontal();
    base_component_option_set_->set_background_style(subwindow_bg);

    secondary_component_text_ = secondary_component_container->create_child<gui::Text_element>("Secondary component", component_text_style);
    secondary_component_text_->set_offset({ 0.0, 4.0 });
    secondary_component_text_->set_size(component_text_size);
    secondary_component_text_->center_text_horizontal();
    secondary_component_text_->register_style(gui::states::selected, component_text_hover_style);
    secondary_component_text_->register_style(gui::states::hover, component_text_hover_style);

    secondary_component_option_set_ = secondary_component_container->create_child<extra_option_set>(option_set_style);
    secondary_component_option_set_->set_position({ 0.0, 30.0 });
    secondary_component_option_set_->center_horizontal();
    secondary_component_option_set_->set_background_style(subwindow_bg);

    tertiary_component_text_ = tertiary_component_container->create_child<gui::Text_element>("Tertiary component", component_text_style);
    tertiary_component_text_->set_offset({ 0.0, 4.0 });
    tertiary_component_text_->set_size(component_text_size);
    tertiary_component_text_->center_text_horizontal();
    tertiary_component_text_->register_style(gui::states::selected, component_text_hover_style);
    tertiary_component_text_->register_style(gui::states::hover, component_text_hover_style);

    tertiary_component_option_set_ = tertiary_component_container->create_child<extra_option_set>(option_set_style);
    tertiary_component_option_set_->set_position({ 0.0, 30.0 });
    tertiary_component_option_set_->center_horizontal();
    tertiary_component_option_set_->set_background_style(subwindow_bg);

    for (std::uint32_t value = 0; value != resources::Color_base::Max_value; ++value)
    {
        auto type = static_cast<resources::Color_base::Type>(value);
        base_component_option_set_->add_option(type, to_string(type));
    }

    for (std::uint32_t value = 0; value != resources::Color_extra::Max_value; ++value)
    {
        auto type = static_cast<resources::Color_extra::Type>(value);

        auto type_string = to_string(type);
        secondary_component_option_set_->add_option(type, type_string);
        tertiary_component_option_set_->add_option(type, type_string);
    }

    auto make_color_picker = [this, font](gui::Element* parent, const utf8_string& header_string)
    {
        gui::Text_style text_style;
        text_style.color = sf::Color(255, 255, 255);
        text_style.font = font;
        text_style.character_size = 15;

        Vector2<double> slider_size{ 100.0, 20.0 };
        gui::Slider_range slider_range{ 0.0, 255.0 };
        auto horizontal = gui::Slider_direction::Horizontal;
        auto slider_background = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 40));

        gui::Integer_input_style color_input_style;
        color_input_style.arrow_area_size = { 20.0, 20.0 };
        color_input_style.text_area_size = { 30.0, 20.0 };
        color_input_style.text_style = text_style;
        color_input_style.text_hover_style = text_style;
        color_input_style.text_hover_style.color = sf::Color(255, 150, 0);

        gui::Integer_input_range color_input_range;
        color_input_range.min = 0;
        color_input_range.max = 255;
        color_input_range.value = 0;

        auto y_position = 30.0;

        auto header_text = parent->create_child<gui::Text_element>(header_string, text_style);
        header_text->set_position({ 20.0, 4.0 });

        auto color_preview = parent->create_child<gui::Element>(Vector2i(20, 20));
        color_preview->set_position({ 170.0, 4.0 });

        auto color_preview_style = gui::make_background_style<gui::Plain_background>(sf::Color());
        color_preview->set_background_style(color_preview_style);

        text_style.color = sf::Color(255, 150, 150);
        auto red = parent->create_child<gui::Text_element>("R", text_style);
        red->set_position({ 20.0, y_position });
        
        auto red_slider = parent->create_child<gui::Slider_bar>(slider_size, horizontal, slider_range);
        red_slider->slider()->set_background_style(slider_background);
        red_slider->set_background_style(slider_background);
        red_slider->set_position({ 40.0, y_position + 2.0 });

        auto red_input = parent->create_child<gui::Integer_input>(color_input_style, color_input_range);
        red_input->set_position({ 160.0, y_position });

        y_position += 30.0;
        
        text_style.color = sf::Color(150, 255, 150);
        auto green = parent->create_child<gui::Text_element>("G", text_style);
        green->set_position({ 20.0, y_position });

        auto green_slider = parent->create_child<gui::Slider_bar>(slider_size, horizontal, slider_range);
        green_slider->slider()->set_background_style(slider_background);
        green_slider->set_background_style(slider_background);
        green_slider->set_position({ 40.0, y_position + 2.0 });

        auto green_input = parent->create_child<gui::Integer_input>(color_input_style, color_input_range);
        green_input->set_position({ 160.0, y_position });

        y_position += 30.0;

        text_style.color = sf::Color(150, 150, 255);
        auto blue = parent->create_child<gui::Text_element>("B", text_style);
        blue->set_position({ 20.0, y_position });

        auto blue_slider = parent->create_child<gui::Slider_bar>(slider_size, horizontal, slider_range);
        blue_slider->slider()->set_background_style(slider_background);
        blue_slider->set_background_style(slider_background);
        blue_slider->set_position({ 40.0, y_position + 2.0 });

        auto blue_input = parent->create_child<gui::Integer_input>(color_input_style, color_input_range);
        blue_input->set_position({ 160.0, y_position });

        Color_picker color_picker;
        color_picker.preview_background = color_preview_style.background;
        color_picker.red_slider = red_slider;
        color_picker.red_input = red_input;
        color_picker.green_slider = green_slider;
        color_picker.green_input = green_input;
        color_picker.blue_slider = blue_slider;
        color_picker.blue_input = blue_input;        

        return color_picker;
    };

    auto primary_color_container = container->create_child<gui::Element>(Vector2i(250, 130));
    primary_color_container->set_background_style(subwindow_bg);
    primary_color_container->set_position({ 300.0, 60.0 });

    auto secondary_color_container = container->create_child<gui::Element>(Vector2i(250, 130));
    secondary_color_container->set_background_style(subwindow_bg);
    secondary_color_container->set_position({ 300.0, 210.0 });

    primary_color_picker_ = make_color_picker(primary_color_container, "Primary color");
    secondary_color_picker_ = make_color_picker(secondary_color_container, "Secondary color");

    auto update_from_slider = [this](const gui::Element& element, double new_value)
    {
        update_color_from_sliders();
    };

    auto update_from_input = [this](const gui::Element& element, std::int32_t new_value)
    {
        update_color_from_input();
    };

    base_component_text_->add_event_handler(gui::events::on_click, 
        [this](const gui::Element& element)
    {
        select_base_component();
    });

    secondary_component_text_->add_event_handler(gui::events::on_click,
        [this](const gui::Element& element)
    {
        select_secondary_component();
    });

    tertiary_component_text_->add_event_handler(gui::events::on_click,
        [this](const gui::Element& element)
    {
        select_tertiary_component();
    });

    primary_color_picker_.red_slider->add_event_handler(gui::events::on_slider_move, update_from_slider);
    primary_color_picker_.green_slider->add_event_handler(gui::events::on_slider_move, update_from_slider);
    primary_color_picker_.blue_slider->add_event_handler(gui::events::on_slider_move, update_from_slider);

    secondary_color_picker_.red_slider->add_event_handler(gui::events::on_slider_move, update_from_slider);
    secondary_color_picker_.green_slider->add_event_handler(gui::events::on_slider_move, update_from_slider);
    secondary_color_picker_.blue_slider->add_event_handler(gui::events::on_slider_move, update_from_slider);

    primary_color_picker_.red_input->add_event_handler(gui::Integer_input::on_value_change, update_from_input);
    primary_color_picker_.green_input->add_event_handler(gui::Integer_input::on_value_change, update_from_input);
    primary_color_picker_.blue_input->add_event_handler(gui::Integer_input::on_value_change, update_from_input);

    secondary_color_picker_.red_input->add_event_handler(gui::Integer_input::on_value_change, update_from_input);
    secondary_color_picker_.green_input->add_event_handler(gui::Integer_input::on_value_change, update_from_input);
    secondary_color_picker_.blue_input->add_event_handler(gui::Integer_input::on_value_change, update_from_input);

    base_component_option_set_->add_event_handler(base_option_set::on_change,
        [this](const base_option_set& option_set, resources::Color_base::Type type)
    {
        set_base_component_type(type);
    });

    secondary_component_option_set_->add_event_handler(extra_option_set::on_change,
        [this](const extra_option_set& option_set, resources::Color_extra::Type type)
    {
        set_secondary_component_type(type);
    });

    tertiary_component_option_set_->add_event_handler(extra_option_set::on_change,
        [this](const extra_option_set& option_set, resources::Color_extra::Type type)
    {
        set_tertiary_component_type(type);
    });

    back->add_event_handler(gui::events::on_click, 
        [this](const gui::Element& element)
    {
        return_to_previous();
    });

    back->add_event_handler(gui::events::on_key_down,
        [this](const gui::Element& element, sf::Keyboard::Key key, unsigned int modifiers)
    {
        if (key == sf::Keyboard::Escape)
        {
            return_to_previous();
        }
    });
    
    document->set_visible(false);

    return document;
}

void ts::states::Color_selection_menu::update_color_preview()
{
    player_color_background_->set_color(color_);
}

void ts::states::Color_selection_menu::update_component_types()
{
    base_component_option_set_->set_value(color_.base_color.type);
    secondary_component_option_set_->set_value(color_.primary_extra.type);
    tertiary_component_option_set_->set_value(color_.secondary_extra.type);
}

void ts::states::Color_selection_menu::select_base_component()
{
    selected_component_ = 1;

    base_component_text_->set_state(gui::states::selected, true);
    secondary_component_text_->set_state(gui::states::selected, false);
    tertiary_component_text_->set_state(gui::states::selected, false);

    update_color_pickers(color_.base_color.primary_color, color_.base_color.secondary_color);
}

void ts::states::Color_selection_menu::select_secondary_component()
{
    selected_component_ = 2;

    base_component_text_->set_state(gui::states::selected, false);
    secondary_component_text_->set_state(gui::states::selected, true);
    tertiary_component_text_->set_state(gui::states::selected, false);    
    
    update_color_pickers(color_.primary_extra.primary_color, color_.primary_extra.secondary_color);
}


void ts::states::Color_selection_menu::select_tertiary_component()
{
    selected_component_ = 3;

    base_component_text_->set_state(gui::states::selected, false);
    secondary_component_text_->set_state(gui::states::selected, false);
    tertiary_component_text_->set_state(gui::states::selected, true);

    update_color_pickers(color_.secondary_extra.primary_color, color_.secondary_extra.secondary_color);
}

void ts::states::Color_selection_menu::set_base_component_type(resources::Color_base::Type type)
{
    color_.base_color.type = type;

    update_color_preview();
}

void ts::states::Color_selection_menu::set_secondary_component_type(resources::Color_extra::Type type)
{
    color_.primary_extra.type = type;

    update_color_preview();
}

void ts::states::Color_selection_menu::set_tertiary_component_type(resources::Color_extra::Type type)
{
    color_.secondary_extra.type = type;

    update_color_preview();
}

void ts::states::Color_selection_menu::update_color_from_input()
{
    primary_color_picker_.red_slider->set_value(primary_color_picker_.red_input->value());
    primary_color_picker_.green_slider->set_value(primary_color_picker_.green_input->value());
    primary_color_picker_.blue_slider->set_value(primary_color_picker_.blue_input->value());

    secondary_color_picker_.red_slider->set_value(secondary_color_picker_.red_input->value());
    secondary_color_picker_.green_slider->set_value(secondary_color_picker_.green_input->value());
    secondary_color_picker_.blue_slider->set_value(secondary_color_picker_.blue_input->value());
}


void ts::states::Color_selection_menu::update_color_from_sliders()
{
    if (update_in_progress_) return;

    switch (selected_component_)
    {
    case 2:
        color_.primary_extra.primary_color.r = static_cast<sf::Uint8>(primary_color_picker_.red_slider->value());
        color_.primary_extra.primary_color.g = static_cast<sf::Uint8>(primary_color_picker_.green_slider->value());
        color_.primary_extra.primary_color.b = static_cast<sf::Uint8>(primary_color_picker_.blue_slider->value());

        color_.primary_extra.secondary_color.r = static_cast<sf::Uint8>(secondary_color_picker_.red_slider->value());
        color_.primary_extra.secondary_color.g = static_cast<sf::Uint8>(secondary_color_picker_.green_slider->value());
        color_.primary_extra.secondary_color.b = static_cast<sf::Uint8>(secondary_color_picker_.blue_slider->value());

        primary_color_picker_.preview_background->set_color(color_.primary_extra.primary_color);
        secondary_color_picker_.preview_background->set_color(color_.primary_extra.secondary_color);
        break;

    case 3:
        color_.secondary_extra.primary_color.r = static_cast<sf::Uint8>(primary_color_picker_.red_slider->value());
        color_.secondary_extra.primary_color.g = static_cast<sf::Uint8>(primary_color_picker_.green_slider->value());
        color_.secondary_extra.primary_color.b = static_cast<sf::Uint8>(primary_color_picker_.blue_slider->value());

        color_.secondary_extra.secondary_color.r = static_cast<sf::Uint8>(secondary_color_picker_.red_slider->value());
        color_.secondary_extra.secondary_color.g = static_cast<sf::Uint8>(secondary_color_picker_.green_slider->value());
        color_.secondary_extra.secondary_color.b = static_cast<sf::Uint8>(secondary_color_picker_.blue_slider->value());

        primary_color_picker_.preview_background->set_color(color_.secondary_extra.primary_color);
        secondary_color_picker_.preview_background->set_color(color_.secondary_extra.secondary_color);
        break;

    default:
        color_.base_color.primary_color.r = static_cast<sf::Uint8>(primary_color_picker_.red_slider->value());
        color_.base_color.primary_color.g = static_cast<sf::Uint8>(primary_color_picker_.green_slider->value());
        color_.base_color.primary_color.b = static_cast<sf::Uint8>(primary_color_picker_.blue_slider->value());

        color_.base_color.secondary_color.r = static_cast<sf::Uint8>(secondary_color_picker_.red_slider->value());
        color_.base_color.secondary_color.g = static_cast<sf::Uint8>(secondary_color_picker_.green_slider->value());
        color_.base_color.secondary_color.b = static_cast<sf::Uint8>(secondary_color_picker_.blue_slider->value());

        primary_color_picker_.preview_background->set_color(color_.base_color.primary_color);
        secondary_color_picker_.preview_background->set_color(color_.base_color.secondary_color);
        break;
    }

    primary_color_picker_.red_input->set_value(static_cast<std::int32_t>(primary_color_picker_.red_slider->value()));
    primary_color_picker_.green_input->set_value(static_cast<std::int32_t>(primary_color_picker_.green_slider->value()));
    primary_color_picker_.blue_input->set_value(static_cast<std::int32_t>(primary_color_picker_.blue_slider->value()));

    secondary_color_picker_.red_input->set_value(static_cast<std::int32_t>(secondary_color_picker_.red_slider->value()));
    secondary_color_picker_.green_input->set_value(static_cast<std::int32_t>(secondary_color_picker_.green_slider->value()));
    secondary_color_picker_.blue_input->set_value(static_cast<std::int32_t>(secondary_color_picker_.blue_slider->value()));

    update_color_preview();
}

void ts::states::Color_selection_menu::update_color_pickers(sf::Color primary_color, sf::Color secondary_color)
{
    update_in_progress_ = true;

    primary_color_picker_.red_slider->set_value(primary_color.r);
    primary_color_picker_.green_slider->set_value(primary_color.g);
    primary_color_picker_.blue_slider->set_value(primary_color.b);

    secondary_color_picker_.red_slider->set_value(secondary_color.r);
    secondary_color_picker_.green_slider->set_value(secondary_color.g);
    secondary_color_picker_.blue_slider->set_value(secondary_color.b);

    primary_color_picker_.preview_background->set_color(primary_color);
    secondary_color_picker_.preview_background->set_color(secondary_color);

    update_in_progress_ = false;
    update_color_from_sliders();
}