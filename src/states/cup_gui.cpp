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
#include "cup_gui.hpp"

#include "cup/cup.hpp"

#include "resources/track.hpp"
#include "game/car_image_generator.hpp"

#include "user_interface/document.hpp"
#include "user_interface/elements/elements.hpp"

#include "gui_definitions/window_template.hpp"
#include "gui_definitions/background_document.hpp"


ts::states::Cup_GUI::Cup_GUI(cup::Cup_interface* cup_interface, gui::Context* context, const resources::Resource_store* resource_store)
    : cup_interface_(cup_interface),
      cup_(cup_interface->cup()),
      context_(context),    
      car_selection_ready_(false)
{
    create_cup_document(context);

    progress_dialog_ = std::make_unique<Loading_progress_dialog>(context);
    car_selection_dialog_ = std::make_unique<Car_selection_dialog>(context, this);
}

void ts::states::Cup_GUI::show()
{
    cup_document_->set_visible(true);

    gui_definitions::show_background_document(context_);
}

void ts::states::Cup_GUI::hide()
{
    cup_document_->set_visible(false);
}

ts::states::Loading_progress_dialog* ts::states::Cup_GUI::progress_dialog()
{
    return progress_dialog_.get();
}

void ts::states::Cup_GUI::set_cup_state_text(utf8_string state_text)
{
    header_text_->set_text(std::move(state_text));
}

void ts::states::Cup_GUI::update(std::size_t frame_duration)
{
    if (car_selection_ready_)
    {
        car_selection_dialog_->show();
        car_selection_ready_ = false;
    }
}

void ts::states::Cup_GUI::return_to_main_menu()
{
    cup_interface_->end_cup();
}

void ts::states::Cup_GUI::apply_car_selection()
{
    cup_interface_->select_cars(car_selection_dialog_->car_selection());
}

void ts::states::Cup_GUI::show_car_selection_dialog()
{
    std::vector<cup::Player_handle> selected_players = cup_->local_players();
    selected_players.erase(std::remove(selected_players.begin(), selected_players.end(), cup::Player_handle()), 
        selected_players.end());

    car_selection_ready_ = false;
    car_selection_dialog_->load(selected_players, cup_->car_list(), cup_->current_track());
}

void ts::states::Cup_GUI::car_selection_dialog_ready()
{
    car_selection_ready_ = true;
}

void ts::states::Cup_GUI::confirm_car_selection()
{
    car_selection_dialog_->hide();
    cup_document_->set_visible(true);

    apply_car_selection();
}

void ts::states::Cup_GUI::output_chat_message(const cup::Composite_message& message)
{
    chatbox_->create_row(message, chatbox_text_style_);
}

void ts::states::Cup_GUI::create_cup_document(gui::Context* context)
{
    cup_document_ = context->create_document("cup-document");
    cup_document_->set_visible(false);

    auto screen_size = context->screen_size();

    const auto& font_library = context->font_library();

    auto monospace = font_library.font_by_name(gui::fonts::Monospace);
    auto sans = font_library.font_by_name(gui::fonts::Sans);

    Vector2<double> window_size(screen_size.x - 80.0, screen_size.y - 60.0);

    

    auto window = gui_definitions::create_styled_window(context, cup_document_.get(), window_size);

    window->center_horizontal();
    window->center_vertical();

    gui::Text_style header_style;
    header_style.color = sf::Color(200, 200, 200);
    header_style.character_size = 20;
    header_style.font = sans;

    header_text_ = window->create_child<gui::Text_element>("", header_style);
    header_text_->center_horizontal();
    header_text_->set_position({ 0.0, 30.0 });

    chatbox_text_style_.font = monospace;
    chatbox_text_style_.character_size = 15;
    chatbox_text_style_.color = sf::Color::White;

    Vector2<double> scroll_pane_size(window_size.x - 80.0, window_size.y - 220.0);

    auto scroll_pane = window->create_child<gui::Scroll_pane>(scroll_pane_size, gui::Scroll_pane_style());
    scroll_pane->set_position({ 40.0, 40.0 });

    gui::Vertical_list_style chatbox_style;
    chatbox_style.row_size.x = window_size.x - 80.0;
    chatbox_style.row_size.y = 0.0;

    chatbox_ = scroll_pane->create_child<chatbox_type>(chatbox_style);

    auto chat_editbox_background = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 25));

    auto slider_background = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 20));

    scroll_pane->vertical_scroll_bar()->slider()->set_background_style(slider_background);
    scroll_pane->vertical_scroll_bar()->set_background_style(slider_background);

    auto chat_editbox = window->create_child<gui::Edit_box>(chatbox_text_style_);
    chat_editbox->set_position({ 40.0, window_size.y - 180.0 });
    chat_editbox->set_size({ window_size.x - 80.0, 20.0 });
    chat_editbox->set_offset({ 10.0, 1.0 });

    chat_editbox->Element::set_background_style(chat_editbox_background);

    gui::Text_style control_style;
    control_style.font = sans;
    control_style.character_size = 20;
    control_style.color = sf::Color(255, 255, 255);

    auto control_hover_style = control_style;
    control_hover_style.color = sf::Color(255, 150, 0);

    Vector2<double> control_size(200.0, 30.0);
    Vector2<double> control_offset(10.0, 0.0);

    if (cup_->cup_type() != cup::Cup_type::Remote)
    {
        auto start_game = window->create_child<gui::Text_element>("Start game!", control_style);
        start_game->set_position({ 60.0, window_size.y - 150.0 });
        start_game->set_size(control_size);
        start_game->set_offset(control_offset);
        start_game->register_style(gui::states::hover, control_hover_style);

        start_game->add_event_handler(gui::events::on_click,
            [this](const gui::Element& element)
        {
            cup_interface_->advance();
        });
    }

    auto quit = window->create_child<gui::Text_element>("Quit", control_style);
    quit->set_position({ 60.0, window_size.y - 120.0 });
    quit->set_size(control_size);
    quit->set_offset(control_offset);
    quit->register_style(gui::states::hover, control_hover_style);

    quit->add_event_handler(gui::events::on_click, 
                            [this](const gui::Element& element)
    {
        return_to_main_menu();
    });

    chat_editbox->add_event_handler(gui::events::on_key_down, 
                                    [this, chat_editbox](const gui::Element& element, sf::Keyboard::Key key, unsigned int key_modifiers)
    {
        if (chat_editbox->active() && key == sf::Keyboard::Return)
        {
            cup_interface_->write_chat_message(chat_editbox->text());
            chat_editbox->reset();
        }
    });
}

ts::states::Loading_progress_dialog::Loading_progress_dialog(gui::Context* context)
{
    create_progress_document(context);
}

void ts::states::Loading_progress_dialog::set_loading_state(utf8_string state)
{
    progress_info_text_->set_text(std::move(state));
}

void ts::states::Loading_progress_dialog::set_progress(double progress)
{
    progress_bar_->set_value(progress);
}

void ts::states::Loading_progress_dialog::set_max_progress(double max_progress)
{
    progress_bar_->set_max_value(max_progress);
}

void ts::states::Loading_progress_dialog::show()
{
    progress_document_->set_visible(true);
}

void ts::states::Loading_progress_dialog::hide()
{
    progress_document_->set_visible(false);
}

void ts::states::Loading_progress_dialog::create_progress_document(gui::Context* context)
{
    progress_document_ = context->create_document("progress-window");

    progress_document_->set_visible(false);

    const auto& font_library = context->font_library();
    auto sans = font_library.font_by_name(gui::fonts::Sans);

    Vector2<double> window_size(320.0, 120.0);

    auto window = gui_definitions::create_styled_window(context, progress_document_.get(), window_size);
    window->center_horizontal();
    window->center_vertical();

    gui::Text_style info_style;
    info_style.font = sans;
    info_style.character_size = 16;
    info_style.color = sf::Color::White;

    progress_info_text_ = window->create_child<gui::Text_element>(utf8_string(), info_style);
    progress_info_text_->center_horizontal();
    progress_info_text_->set_position({ 0.0, 30.0 });

    gui::Progress_bar_style progress_bar_style;
    progress_bar_style.bar_style = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 80));
    progress_bar_style.completed_style = gui::make_background_style<gui::Plain_background>(sf::Color(230, 110, 0));

    Vector2<double> progress_bar_size(240.0, 24.0);

    progress_bar_ = window->create_child<gui::Progress_bar>(progress_bar_size, progress_bar_style);
    progress_bar_->set_position({ 40.0, 60 });
}

ts::states::Car_selection_dialog::Car_selection_dialog(gui::Context* context, Cup_GUI* cup_gui)
: cup_gui_(cup_gui)
{
    create_car_selection_dialog(context);
}

void ts::states::Car_selection_dialog::load(std::vector<cup::Player_handle> selected_players, std::vector<resources::Car_handle> possible_cars, resources::Track_handle track_handle)
{
    selected_players_ = std::move(selected_players);
    possible_cars_ = std::move(possible_cars);
    track_handle_ = track_handle;

    std::async(std::launch::async, [this]() { load_dialog(); });
}

void ts::states::Car_selection_dialog::show()
{
    car_selection_document_->set_visible(true);
}

void ts::states::Car_selection_dialog::hide()
{
    car_selection_document_->set_visible(false);

    
}

void ts::states::Car_selection_dialog::create_car_selection_dialog(gui::Context* context)
{
    car_selection_document_ = context->create_document("car-selection");
    car_selection_document_->set_visible(false);

    auto& font_library = context->font_library();

    auto font = font_library.font_by_name(gui::fonts::Sans);

    Vector2i window_size(600, 440);

    auto window = gui_definitions::create_styled_window(context, car_selection_document_.get(), window_size);
    window->center_horizontal();
    window->center_vertical();

    gui::Text_style header_text_style;
    header_text_style.font = font;
    header_text_style.color = sf::Color(255, 255, 255);
    header_text_style.character_size = 18;

    auto header_text = window->create_child<gui::Text_element>("Car Selection", header_text_style);
    header_text->set_position({ 0.0, 30.0 });
    header_text->center_horizontal();

    gui::Vertical_list_style selection_list_style;
    selection_list_style.row_size = { 480.0, 40.0 };
    selection_list_style.item_style = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 10));

    selection_list_ = window->create_child<gui::Vertical_list<gui::Element>>(selection_list_style);
    selection_list_->set_position({ 40.0, 70.0 });

    auto button_style = header_text_style;
    button_style.character_size = 16;
    
    auto button_hover_style = button_style;
    button_hover_style.color = sf::Color(255, 150, 0);

    auto confirm_button = window->create_child<gui::Text_element>("Confirm", button_style);
    confirm_button->set_position({ 60.0, window_size.y - 60.0 });
    confirm_button->set_size({ 80.0, 25.0 });
    confirm_button->register_style(gui::states::hover, button_hover_style);

    confirm_button->add_event_handler(gui::events::on_click, 
        [this](const gui::Element& element)
    {
        confirm();
    });

    confirm_button->add_event_handler(gui::events::on_key_down, 
        [this](const gui::Element& element, sf::Keyboard::Key key, unsigned int key_modifiers)
    {
        if (key == sf::Keyboard::Return || key == sf::Keyboard::Escape)
        {
            confirm();
        }
    });
}

void ts::states::Car_selection_dialog::load_car_textures()
{
    game::Car_image_generator image_generator;

    std::size_t player_index = 0;

    std::vector<sf::Image> images;
    sf::Image compound_image;

    player_car_textures_.clear();
    texture_mapping_.clear();

    player_car_textures_.reserve(selected_players_.size());
    texture_mapping_.reserve(selected_players_.size());

    for (auto player : selected_players_)
    {
        if (images.empty())
        {
            Vector2u target_size;
            std::int32_t position_x = 0;

            for (auto& car : possible_cars_)
            {
                images.push_back(image_generator(*car, player->color, game::Car_image_generator::Single_frame));

                auto& image = images.back();
                auto image_size = image.getSize();

                texture_mapping_.push_back(Int_rect(position_x, 0, image_size.x, image_size.y));
                position_x += image_size.x + 2;

                target_size.x = position_x;
                target_size.y = std::max(target_size.y, image_size.y);
            }

            compound_image.create(target_size.x, target_size.y, sf::Color::Transparent);
        }

        else
        {
            std::size_t image_index = 0;

            for (auto& car : possible_cars_)
            {
                images[image_index++] = image_generator(*car, player->color, game::Car_image_generator::Single_frame);
            }
        }

        std::size_t image_index = 0;
        for (auto& image : images)
        {
            auto& bounds = texture_mapping_[image_index++];
            compound_image.copy(image, bounds.left, bounds.top);
        }

        player_car_textures_.push_back(std::make_shared<graphics::Texture>(compound_image));
    }
}

void ts::states::Car_selection_dialog::confirm()
{
    cup_gui_->confirm_car_selection();
}

const std::vector<ts::cup::Car_selection>& ts::states::Car_selection_dialog::car_selection() const
{
    return car_selection_;
}

void ts::states::Car_selection_dialog::load_dialog()
{
    load_car_textures();

    auto& font_library = context_->font_library();
    auto font = font_library.font_by_name(gui::fonts::Sans);

    gui::Text_style text_style;
    text_style.color = sf::Color::White;
    text_style.font = font;
    text_style.character_size = 15;

    gui::Option_set_style car_set_style;
    car_set_style.arrow_area_size = { 20.0, 36.0 };
    car_set_style.text_area_size = { 180.0, 36.0 };
    car_set_style.text_style = text_style;
    car_set_style.text_hover_style = text_style;
    car_set_style.text_hover_style.color = sf::Color(255, 150, 0);

    auto get_car_index = [this](resources::Car_handle car_handle) -> std::size_t
    {
        auto it = std::find(possible_cars_.begin(), possible_cars_.end(), car_handle);
        if (it == possible_cars_.end()) return 0;

        return it - possible_cars_.begin();
    };

    selection_list_->clear();
    car_selection_.clear();

    std::size_t player_index = 0;
    for (auto player_handle : selected_players_)
    {
        auto row = selection_list_->create_row();

        auto car_index = get_car_index(player_handle->car);

        const auto& car = possible_cars_[car_index];

        const auto& car_texture = player_car_textures_[player_index];
        const auto& bounds = texture_mapping_[car_index];

        auto name_text = row->create_child<gui::Text_element>(player_handle->nickname, text_style);
        name_text->set_position({ 20.0, 8.0 });

        auto car_preview = row->create_child<gui::Element>(Vector2i(36, 36));
        car_preview->set_position({ 180.0, 2.0 });

        const auto centered = gui::Textured_background::Mode::Centered;

        auto car_background = gui::make_background_style<gui::Textured_background>(car_texture, bounds, centered);

        auto scale = 2.0 / car->image_scale;
        car_background.background->set_scale({ scale, scale });

        car_preview->set_background_style(car_background);

        auto car_option_set = row->create_child<gui::Option_set<std::size_t>>(car_set_style);
        car_option_set->set_position({ 240.0, 8.0 });

        for (std::size_t index = 0; index != possible_cars_.size(); ++index)
        {
            car_option_set->add_option(index, possible_cars_[index]->car_name);
        }

        auto textured_background = car_background.background;

        car_option_set->add_event_handler(gui::Option_set<std::size_t>::on_change, 
            [=](const gui::Option_set<std::size_t>& option_set, std::size_t new_car_index)
        {
            auto car = possible_cars_[new_car_index];

            car_selection_[player_index].player_handle = player_handle;
            car_selection_[player_index].car_handle = car;

            const auto& sub_rect = texture_mapping_[new_car_index];
            textured_background->set_sub_rect(sub_rect);

            auto scale = 2.0 / car->image_scale;
            textured_background->set_scale({ scale, scale });
        });

        car_option_set->set_value(car_index);

        cup::Car_selection car_selection;
        car_selection.player_handle = player_handle;
        car_selection.car_handle = car;

        car_selection_.push_back(car_selection);

        ++player_index;
    }

    cup_gui_->car_selection_dialog_ready();
}