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

#include "client/client_interface.hpp"
#include "client/client_messages.hpp"
#include "cup/cup_messages.hpp"
#include "cup/cup.hpp"

#include "resources/resource_store.hpp"
#include "resources/track_handle.hpp"
#include "resources/car_handle.hpp"
#include "resources/car_store.hpp"
#include "resources/car_image_generator.hpp"

#include "user_interface/document.hpp"
#include "user_interface/elements/elements.hpp"

#include "gui_definitions/window_template.hpp"
#include "gui_definitions/background_document.hpp"

namespace ts
{
    namespace states
    {
        class Loading_progress_dialog
        {
        public:
            Loading_progress_dialog(gui::Context* context);

            void show();
            void hide();

            bool visible() const;

            void set_loading_state(utf8_string state);
            void set_progress(double progress);
            void set_max_progress(double max_progress);

        private:
            void create_progress_document(gui::Context* context);
            
            gui::Text_element* progress_info_text_ = nullptr;
            gui::Progress_bar* progress_bar_ = nullptr;

            gui::Document_handle progress_document_;
        };
        
        class Car_selection_dialog
        {
        public:
            Car_selection_dialog(gui::Context* context, impl::Cup_GUI* cup_gui, const resources::Resource_store* resource_store);

            void load(std::vector<cup::Player_definition> local_players, std::vector<resources::Car_identifier> possible_cars, 
                      resources::Track_handle track_handle);

            void show();
            void hide();

            const std::vector<std::pair<cup::Player_definition, std::uint32_t>>& car_selection() const;

        private:
            void load_dialog();
            void load_car_textures();
            void confirm();

            void create_car_selection_dialog(gui::Context* context);

            impl::Cup_GUI* cup_gui_;
            gui::Context* context_;
            const resources::Resource_store* resource_store_;

            std::vector<cup::Player_definition> local_players_;
            std::vector<resources::Car_identifier> possible_cars_;
            std::vector<resources::Car_handle> car_handles_;
            std::unordered_map<cup::Player_id, resources::Car_identifier> selection_cache_;
            resources::Track_handle track_handle_;

            std::shared_ptr<graphics::Texture> car_preview_texture_;
            std::map<std::pair<std::uint32_t, std::uint32_t>, Int_rect> texture_mapping_;

            std::vector<std::pair<cup::Player_definition, std::uint32_t>> car_selection_;

            std::future<void> loading_future_;

            gui::Vertical_list<gui::Element>* selection_list_;

            gui::Document_handle car_selection_document_;
        };
    }
}

class ts::states::impl::Cup_GUI
    : private client::Message_listener
{
public:
    Cup_GUI(const client::Client_interface* client_interface, gui::Context* context, const resources::Resource_store* resource_store);

    void confirm_car_selection();
    void set_car_selection_dialog_ready();

    void show();
    void hide();

    bool quit_event_pending() const;

    void update(std::size_t frame_duration);
    void set_cup_state_text(utf8_string text);

    void show_car_selection_dialog(const std::vector<resources::Car_identifier>& car_list);

    void show_progress_dialog();
    void hide_progress_dialog();

    void set_loading_progress(double progress);
    void set_loading_progress_text(utf8_string text);

    void output_chat_message(const cup::Chat_message& message);     

private:
    virtual void handle_message(const client::Server_message& message) override;
    void handle_chatbox_output_message(const client::Message& message);
    void handle_car_selection_initiation_message(const client::Message& message);
    void handle_load_error_message(const client::Message& message);

    void show_menu_background();

    void create_cup_document(gui::Context* context);

    void apply_car_selection();
    void return_to_main_menu();
            
    const client::Client_interface* client_interface_;
    const cup::Cup* cup_;
    gui::Context* context_;

    gui::Text_element* header_text_;
    using chatbox_type = gui::Vertical_list<gui::Text_element>;
    gui::Text_style chatbox_text_style_;
    chatbox_type* chatbox_;

    std::atomic<bool> car_selection_ready_;
    bool quit_event_pending_ = false;

    gui::Document_handle cup_document_;

    std::unique_ptr<Car_selection_dialog> car_selection_dialog_;
    std::unique_ptr<Loading_progress_dialog> progress_dialog_; 
};


ts::states::impl::Cup_GUI::Cup_GUI(const client::Client_interface* client_interface, gui::Context* context, const resources::Resource_store* resource_store)
: Message_listener(client_interface->message_center()), 
  client_interface_(client_interface),
  cup_(client_interface->cup()),
  context_(context)
{
    create_cup_document(context);

    car_selection_dialog_ = std::make_unique<Car_selection_dialog>(context, this, resource_store);
    progress_dialog_ = std::make_unique<Loading_progress_dialog>(context);

}

ts::states::Cup_GUI::Cup_GUI(const client::Client_interface* client_interface, gui::Context* context, const resources::Resource_store* resource_store)
: impl_(std::make_unique<impl::Cup_GUI>(client_interface, context, resource_store))
{
}

ts::states::Cup_GUI::~Cup_GUI()
{
}

void ts::states::impl::Cup_GUI::handle_message(const client::Server_message& server_message)
{
    using Msg = cup::Message_type;

    if (server_message.message_type == client::Message_type::Reliable)
    {
        const auto& message = server_message.message;
        switch (message.type())
        {
        case Msg::chatbox_output:
            handle_chatbox_output_message(message);
            break;

        case Msg::car_selection_initiation:
            handle_car_selection_initiation_message(message);
            break;

        case Msg::load_error:
            handle_load_error_message(message);
            break;
        }
    }
}

void ts::states::impl::Cup_GUI::handle_chatbox_output_message(const client::Message& message)
{
    auto output = cup::parse_chatbox_output_message(message);
    output_chat_message(output.message);
}

void ts::states::impl::Cup_GUI::handle_car_selection_initiation_message(const client::Message& message)
{
    auto car_info = cup::parse_car_selection_initiation_message(message);

    show_car_selection_dialog(car_info.cars);
}

void ts::states::impl::Cup_GUI::handle_load_error_message(const client::Message& message)
{
    auto error_message = cup::parse_load_error_message(message);

    cup::Chat_message composite_message;
    composite_message.append("Error loading stage: " + error_message.error_string, sf::Color(220, 0, 0));

    output_chat_message(composite_message);

    hide_progress_dialog();
}

void ts::states::impl::Cup_GUI::show()
{
    cup_document_->set_visible(true);

    gui_definitions::show_background_document(context_);
}

void ts::states::impl::Cup_GUI::hide()
{
    cup_document_->set_visible(false);
}

bool ts::states::impl::Cup_GUI::quit_event_pending() const
{
    return quit_event_pending_;
}

void ts::states::impl::Cup_GUI::show_progress_dialog()
{
    cup_document_->set_active(false);

    progress_dialog_->show();
}

void ts::states::impl::Cup_GUI::hide_progress_dialog()
{
    cup_document_->set_active(true);

    progress_dialog_->hide();
}

void ts::states::impl::Cup_GUI::set_loading_progress(double progress)
{
    progress_dialog_->set_progress(progress);
}

void ts::states::impl::Cup_GUI::set_loading_progress_text(utf8_string text)
{
    progress_dialog_->set_loading_state(std::move(text));
}

void ts::states::impl::Cup_GUI::set_cup_state_text(utf8_string state_text)
{
    header_text_->set_text(std::move(state_text));
}

void ts::states::impl::Cup_GUI::update(std::size_t frame_duration)
{
    if (car_selection_ready_)
    {
        car_selection_dialog_->show();
        car_selection_ready_ = false;

        cup_document_->set_active(false);
    }
}

void ts::states::impl::Cup_GUI::return_to_main_menu()
{
    quit_event_pending_ = true;
}

void ts::states::impl::Cup_GUI::apply_car_selection()
{
    std::vector<client::Car_selection> car_selection;
    for (const auto& entry : car_selection_dialog_->car_selection())
    {
        client::Car_selection selection;
        selection.player_id = entry.first.handle;
        selection.car_index = entry.second;

        car_selection.push_back(selection);
    }

    client_interface_->select_cars(car_selection);
}

void ts::states::impl::Cup_GUI::show_car_selection_dialog(const std::vector<resources::Car_identifier>& car_list)
{
    std::vector<cup::Player_definition> local_players;
    for (const auto& player : cup_->local_players())
    {
        local_players.push_back(*player);
    }

    car_selection_ready_ = false;
    car_selection_dialog_->load(local_players, car_list, resources::Track_handle());
}

void ts::states::Cup_GUI::set_cup_state_text(utf8_string text)
{
    impl_->set_cup_state_text(std::move(text));
}

void ts::states::Cup_GUI::show_progress_dialog()
{
    impl_->show_progress_dialog();
}

void ts::states::Cup_GUI::hide_progress_dialog()
{
    impl_->hide_progress_dialog();
}

void ts::states::Cup_GUI::set_loading_progress(double progress)
{
    impl_->set_loading_progress(progress);
}

void ts::states::Cup_GUI::set_loading_progress_text(utf8_string text)
{
    impl_->set_loading_progress_text(std::move(text));
}

bool ts::states::Cup_GUI::quit_event_pending() const
{
    return impl_->quit_event_pending();
}

void ts::states::Cup_GUI::output_chat_message(const cup::Chat_message& message)
{
    impl_->output_chat_message(message);
}


void ts::states::impl::Cup_GUI::set_car_selection_dialog_ready()
{
    car_selection_ready_ = true;
}

void ts::states::impl::Cup_GUI::confirm_car_selection()
{
    car_selection_dialog_->hide();

    cup_document_->set_visible(true);
    cup_document_->set_active(true);

    apply_car_selection();
}

void ts::states::impl::Cup_GUI::output_chat_message(const cup::Chat_message& message)
{
    chatbox_->create_row(message, chatbox_text_style_);
}

void ts::states::impl::Cup_GUI::create_cup_document(gui::Context* context)
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

    if (cup_->is_local())
    {
        auto start_game = window->create_child<gui::Text_element>("Start game!", control_style);
        start_game->set_position({ 60.0, window_size.y - 150.0 });
        start_game->set_size(control_size);
        start_game->set_offset(control_offset);
        start_game->register_style(gui::states::hover, control_hover_style);

        start_game->add_event_handler(gui::events::on_click,
            [this](const gui::Element& element)
        {
            client_interface_->request_advance();
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
            client_interface_->write_message(chat_editbox->text());
            chat_editbox->reset();
        }
    });
}

void ts::states::Cup_GUI::show()
{
    impl_->show();
}

void ts::states::Cup_GUI::update(std::size_t frame_duration)
{
    impl_->update(frame_duration);
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

ts::states::Car_selection_dialog::Car_selection_dialog(gui::Context* context, impl::Cup_GUI* cup_gui, 
                                                       const resources::Resource_store* resource_store)
: cup_gui_(cup_gui),
  context_(context),
  resource_store_(resource_store)
{
    create_car_selection_dialog(context);
}

void ts::states::Car_selection_dialog::load(std::vector<cup::Player_definition> local_players, std::vector<resources::Car_identifier> possible_cars,
                                            resources::Track_handle track_handle)
{
    const auto& car_store = resource_store_->car_store();

    local_players_ = std::move(local_players);
    possible_cars_ = std::move(possible_cars);
    track_handle_ = track_handle;    

    car_handles_.clear();    
    for (const auto& car_identifier : possible_cars_)
    {
        car_handles_.push_back(car_store.get_car_by_name(car_identifier.car_name));
    };

    car_selection_.resize(local_players_.size());

    std::uint32_t player_index = 0;
    for (const auto& player : local_players_)
    {
        car_selection_[player_index].first = player;
        car_selection_[player_index].second = 0;
        ++player_index;
    }

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
    

    car_preview_texture_ = nullptr;
    texture_mapping_.clear();

    const auto& car_store = resource_store_->car_store();

    // Generate a compound texture with all player+car combinations.
    using resources::Car_image_generator;
    Car_image_generator image_generator;

    std::vector<sf::Image> temp_images;
    temp_images.reserve(local_players_.size() * possible_cars_.size());
    
    Vector2u total_size;
    std::size_t x_position = 0;
    std::size_t player_index = 0;
    for (auto player : local_players_)
    {
        std::size_t car_index = 0;
        for (auto& car : possible_cars_)
        {
            auto mapping_pair = std::make_pair(player_index, car_index);
            temp_images.emplace_back();
            auto& temp_image = temp_images.back();

            if (auto car_handle = car_store.get_car_by_name(car.car_name))
            {
                temp_image = image_generator(*car_handle, player.color, Car_image_generator::Single_frame);
                auto image_size = temp_image.getSize();

                texture_mapping_[mapping_pair] = Int_rect(x_position, 0, image_size.x, image_size.y);

                x_position += image_size.x;
                total_size.x = x_position;
                total_size.y = std::max(image_size.y, total_size.y);                

                // Padding
                x_position += 2;
            }

            else
            {
                // No preview available. Empty mapping.
                texture_mapping_[mapping_pair] = Int_rect();
                
            }

            ++car_index;
        }

        ++player_index;
    }

    sf::Image compound_image;
    compound_image.create(total_size.x, total_size.y);

    for (const auto& mapping : texture_mapping_)
    {
        const auto& indices = mapping.first;
        const auto& rect = mapping.second;
        auto flat_index = indices.first * possible_cars_.size() + indices.second;
        const auto& source = temp_images[flat_index];

        compound_image.copy(source, rect.left, rect.top);
    }

    car_preview_texture_ = std::make_shared<graphics::Texture>(compound_image);
}

void ts::states::Car_selection_dialog::confirm()
{
    cup_gui_->confirm_car_selection();
}

const std::vector<std::pair<ts::cup::Player_definition, std::uint32_t>>& ts::states::Car_selection_dialog::car_selection() const
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

    selection_list_->clear();

    std::uint32_t player_index = 0;
    for (auto player : local_players_)
    {
        // Create a row for every player.
        auto row = selection_list_->create_row();

        // Show the player name next to the car preview
        auto name_text = row->create_child<gui::Text_element>(player.nickname, text_style);
        name_text->set_position({ 20.0, 8.0 });

        auto car_preview = row->create_child<gui::Element>(Vector2i(36, 36));
        car_preview->set_position({ 180.0, 2.0 });

        const auto centered = gui::Textured_background::Mode::Centered;
        auto car_background = gui::make_background_style<gui::Textured_background>(car_preview_texture_, Int_rect(), centered);
        car_preview->set_background_style(car_background);

        auto car_option_set = row->create_child<gui::Option_set<std::size_t>>(car_set_style);
        car_option_set->set_position({ 240.0, 8.0 });

        for (std::size_t index = 0; index != possible_cars_.size(); ++index)
        {
            car_option_set->add_option(index, possible_cars_[index].car_name);
        }

        auto update_selection = [=](const gui::Option_set<std::size_t>& option_set, std::size_t new_car_index)
        {
            auto mapping_pair = std::make_pair(player_index, new_car_index);
            auto mapping_it = texture_mapping_.find(mapping_pair);
            if (mapping_it != texture_mapping_.end())
            {
                const auto& car_handle = car_handles_[new_car_index];

                const auto& sub_rect = mapping_it->second;
                car_background.background->set_sub_rect(sub_rect);
                
                auto scale = car_handle ? 2.0 / car_handle->image_scale : 1.0;
                car_background.background->set_scale({ scale, scale });

                car_selection_[player_index].second = new_car_index;
            }
        };

        car_option_set->add_event_handler(gui::Option_set<std::size_t>::on_change, update_selection);

        // Use the car this player previously chose
        std::uint32_t car_index = 0;
        auto previous_selection = selection_cache_.find(player.handle);
        if (previous_selection != selection_cache_.end())
        {
            // Find the index of the previous selection
            auto search_result = std::find(possible_cars_.begin(), possible_cars_.end(), previous_selection->second);
            if (search_result != possible_cars_.end())
            {
                car_index = search_result - possible_cars_.begin();
            }
        }

        // 'Initialize' the car preview
        car_option_set->set_value(car_index);
        update_selection(*car_option_set, car_index);

        ++player_index;
    }

    cup_gui_->set_car_selection_dialog_ready();
}