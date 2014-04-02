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

#include "main_menu.hpp"
#include "graphics/fonts/sans.hpp"
#include "graphics/fonts/font.hpp"

#include "user_interface/context.hpp"
#include "user_interface/elements/text_button.hpp"
#include "user_interface/elements/vertex_array.hpp"
#include "user_interface/elements/rectangle_region.hpp"
#include "user_interface/elements/colored_area.hpp"

#include "resources/track_store.hpp"
#include "resources/track.hpp"

#include "resources/car_store.hpp"
#include "resources/pattern.hpp"

#include "local_cup.hpp"

#include "core/utility.hpp"

namespace ts
{
    namespace states
    {
        const sf::Color header_text_color(180, 180, 180);

        const sf::Color menu_item_color(255, 255, 255);
        const sf::Color menu_item_hover_color(255, 150, 0);
        const unsigned main_menu_character_size = 24U;

        gui::Element make_navigation_menu(Vector2i area_size, unsigned character_size, std::initializer_list<std::string> items)
        {
            gui::Element navigation_menu;

            const auto& font = fonts::get_font_by_type<fonts::Sans>();

            Vector2i position;

            for (const auto& item : items)
            {
                auto child = navigation_menu.create_child<gui::elements::Text_button>(area_size, item, font, character_size);
                child->set_color(menu_item_color);
                child->set_hover_color(menu_item_hover_color);

                child->set_position(position);                

                position.y += area_size.y;
            }

            return navigation_menu;
        }

        gui::Element make_header_text(const std::string& text, unsigned character_size)
        {
            gui::Element dummy_element;
            const auto& font = fonts::get_font_by_type<fonts::Sans>();

            auto child = dummy_element.create_child<gui::elements::Text_button>(Vector2i(0, 0), text, font, character_size);
            child->set_color(header_text_color);
            return dummy_element;
        }

        class Main_menu_scene
            : public gui::Scene
        {
        public:
            Main_menu_scene(Main_menu* main_menu);

            void update();
            virtual void render(graphics::Render_target& render_target) override;

        private:
            Main_menu* main_menu_;
            gui::Element navigation_menu_;
        };

        class Players_scene
            : public gui::Scene
        {
        public:
            Players_scene(Main_menu* main_menu);

            void update();
            virtual void render(graphics::Render_target& render_target) override;

        private:
            void show_player_selection_list(controls::Slot slot);
            void return_to_player_setup();

            Main_menu* main_menu_;
            gui::Element container_;
            gui::Element* main_;

            gui::Element* player_selection_list_;
            gui::Element* return_from_player_selection_;

            gui::Element* player_slots_[4];
            gui::Element* back_to_main_menu_;

            controls::Slot selection_slot_;

            enum class State
            {
                Main, Select
            };

            State state_ = State::Main;
        };

        class Controls_menu
        {
        public:
            Controls_menu(resources::Input_settings* input_settings);            

            void render(graphics::Render_target& render_target, graphics::Render_states render_states);
            void update(const gui::mouse::State& mouse_state);
            void handle_event(const sf::Event& event);

            void set_position(Vector2i position);

        private:
            using control_map_type = std::map<std::pair<controls::Slot, controls::Control>, controls::Key>;
            control_map_type control_map_;
            control_map_type convert_key_mapping(const controls::Key_mapping& key_mapping);

            void set_current_slot(controls::Slot slot_id);
            void update_text_displays();
            sf::Keyboard::Key get_bound_key(controls::Slot slot, controls::Control control);

            gui::Element self_;

            controls::Slot slot_id_;
            gui::elements::Text_button* slot_id_text_;
            gui::Element* next_slot_button_;
            gui::Element* prev_slot_button_;

            gui::elements::Text_button* accelerate_key_;
            gui::elements::Text_button* brake_key_;
            gui::elements::Text_button* left_key_;
            gui::elements::Text_button* right_key_;
            gui::elements::Text_button* fire_key_;
            gui::elements::Text_button* alt_fire_key_;

            controls::Control current_input_;
            resources::Input_settings* input_settings_;
        };

        class Settings_scene
            : public gui::Scene
        {
        public:
            enum class State
            {
                None, General, Video, Audio, Controls, Network
            };

            Settings_scene(Main_menu* main_menu);

            void update();
            virtual void render(graphics::Render_target& render_target) override;

            void handle_event(const sf::Event& event);

            void return_to_main_menu();
            void switch_to(State state);

        private:
            Main_menu* main_menu_;
            gui::Element header_text_;
            gui::Element navigation_menu_;
            Controls_menu controls_menu_;
            State state_ = State::None;
        };
    }
}


/*****************************************************************
                            MAIN MENU
*****************************************************************/

ts::states::Main_menu::Main_menu(state_machine_type* state_machine, gui::Context* gui_context,
                                 resources::Resource_store* resource_store)
: gui::State(state_machine, gui_context, resource_store),
  main_menu_scene_(std::make_unique<Main_menu_scene>(this)),
  settings_scene_(std::make_unique<Settings_scene>(this)),
  players_scene_(std::make_unique<Players_scene>(this))
{
    settings_scene_->set_visible(false);
    players_scene_->set_visible(false);
}


void ts::states::Main_menu::handle_event(const sf::Event& event)
{
    if (settings_scene_->visible()) settings_scene_->handle_event(event);
}

void ts::states::Main_menu::return_to_main_menu()
{
    main_menu_scene_->set_visible(true);
}

void ts::states::Main_menu::update(std::size_t frame_duration)
{
    main_menu_scene_->update();
    settings_scene_->update();
    players_scene_->update();
}

void ts::states::Main_menu::render(graphics::Render_target& render_target)
{
    render_background(render_target);

    main_menu_scene_->render_if_visible(render_target);
    settings_scene_->render_if_visible(render_target);
    players_scene_->render_if_visible(render_target);
}

void ts::states::Main_menu::show_settings_menu()
{
    settings_scene_->set_visible(true);
    main_menu_scene_->set_visible(false);
}

void ts::states::Main_menu::show_players_menu()
{
    players_scene_->set_visible(true);
    main_menu_scene_->set_visible(false);
}

void ts::states::Main_menu::quit()
{
    state_machine()->clear();
}

ts::states::Main_menu_scene::Main_menu_scene(Main_menu* main_menu)
: gui::Scene(main_menu->context()),
  main_menu_(main_menu),
  navigation_menu_(make_navigation_menu(Vector2i(160, 28), 20, { "Players", "Local Game", "Network Game", "Editor", "Settings", "Quit" }))
{
    auto screen_size = context()->screen_size();
    navigation_menu_.set_position(40, screen_size.y - 220);
}


void ts::states::Main_menu_scene::update()
{
    if (visible()) 
    {
        auto mouse_state = context()->mouse_state();
        navigation_menu_.update(mouse_state);
    }
}

void ts::states::Main_menu_scene::render(graphics::Render_target& render_target)
{
    navigation_menu_.render(render_target, graphics::Render_states());

    auto& items = navigation_menu_.children();
    auto& players = *items[0];
    auto& local_game = *items[1];
    auto& network_game = *items[2];
    auto& editor = *items[3];
    auto& settings = *items[4];
    auto& quit = *items[5];

    if (players.clicked())
    {
        players.reset_state();
        main_menu_->show_players_menu();
    }

    if (settings.clicked())
    {
        settings.reset_state();
        main_menu_->show_settings_menu();
    }

    if (quit.clicked())
    {
        quit.reset_state();
        main_menu_->quit();
    }
}

/*****************************************************************
                            PLAYERS
*****************************************************************/

ts::states::Players_scene::Players_scene(Main_menu* main_menu)
: gui::Scene(main_menu->context()),
  main_menu_(main_menu)
{
    auto screen_size = context()->screen_size();

    container_.create_child<gui::elements::Colored_area>(Vector2i(400, 440), sf::Color(255, 255, 255, 20));
    container_.set_position(200, screen_size.y - 460);

    main_ = container_.create_child<gui::Element>();
    player_selection_list_ = container_.create_child<gui::Element>();

    auto area_size = Vector2i(200, 20);

    const auto& font = fonts::get_font_by_type<fonts::Sans>();
    unsigned character_size = 13;

    auto header = main_->create_child<gui::elements::Text_button>(Vector2i(), "Player Setup", font, 16);
    header->set_color(sf::Color(180, 180, 180));
    header->set_position(120, 10);

    header = player_selection_list_->create_child<gui::elements::Text_button>(Vector2i(), "Select Player", font, 16);
    header->set_color(sf::Color(180, 180, 180));
    header->set_position(120, 10);

    Vector2i position(20, 45);
    for (int slot = 0; slot != 4; ++slot)
    {
        const auto string = "Player " + std::to_string(slot + 1);

        auto text = main_->create_child<gui::elements::Text_button>(area_size, string, font, character_size);
        text->set_position(position);

        text->set_color(sf::Color::White);
        text->set_hover_color(sf::Color(255, 150, 0));

        text = main_->create_child<gui::elements::Text_button>(area_size, "", font, character_size);
        text->set_offset(Vector2i(150, 0));
        text->set_color(sf::Color::White);
        text->set_hover_color(sf::Color(255, 150, 0));
        text->set_position(position);

        player_slots_[slot] = text;
        position.y += 30;
    }

    auto back = main_->create_child<gui::elements::Text_button>(Vector2i(70, 25), "Back", font, 18);
    back->set_color(sf::Color::White);
    back->set_hover_color(sf::Color(255, 150, 0));
    back->set_position(40, 400);
    back->set_offset(Vector2i(10, 0));
    back_to_main_menu_ = back;

    back = player_selection_list_->create_child<gui::elements::Text_button>(Vector2i(70, 25), "Back", font, 18);
    back->set_color(sf::Color::White);
    back->set_hover_color(sf::Color(255, 150, 0));
    back->set_position(40, 400);
    back->set_offset(Vector2i(10, 0));
    return_from_player_selection_ = back;

    player_selection_list_->set_visible(false);
}

void ts::states::Players_scene::render(graphics::Render_target& render_target)
{
    container_.render(render_target, graphics::Render_states());
}

void ts::states::Players_scene::update()
{
    container_.update(context()->mouse_state());

    if (state_ == State::Main)
    {
        if (back_to_main_menu_->clicked())
        {
            set_visible(false);
            back_to_main_menu_->reset_state();
            main_menu_->return_to_main_menu();
        }

        for (auto slot = 0; slot != 4; ++slot)
        {
            if (player_slots_[slot]->clicked())
            {
                show_player_selection_list(slot);
            }
        }
    }

    else if (state_ == State::Select)
    {
        if (return_from_player_selection_->clicked())
        {
            return_to_player_setup();
        }
    }
}

void ts::states::Players_scene::return_to_player_setup()
{
    state_ = State::Main;
    main_->set_visible(true);
    player_selection_list_->set_visible(false);
}

void ts::states::Players_scene::show_player_selection_list(controls::Slot slot)
{
    main_->set_visible(false);
    player_selection_list_->set_visible(true);

    state_ = State::Select;
    selection_slot_ = slot;
}


/*****************************************************************
                            SETTINGS
*****************************************************************/

ts::states::Settings_scene::Settings_scene(Main_menu* main_menu)
: gui::Scene(main_menu->context()),
main_menu_(main_menu),
navigation_menu_(make_navigation_menu(Vector2i(160, 28), 20, { "General", "Controls", "Video", "Audio", "Network", "Back" })),
header_text_(make_header_text("Settings", 16)),
controls_menu_(&main_menu->resource_store()->settings.input_settings)
{
    auto screen_size = context()->screen_size();
    navigation_menu_.set_position(40, screen_size.y - 220);

    header_text_.set_position(50, navigation_menu_.position().y - 28);

    controls_menu_.set_position(Vector2i(200, screen_size.y - 460));
}

void ts::states::Settings_scene::switch_to(State state)
{
    state_ = state;
}

void ts::states::Settings_scene::render(graphics::Render_target& render_target)
{
    header_text_.render(render_target, graphics::Render_states());

    navigation_menu_.render(render_target, graphics::Render_states());

    switch (state_)
    {
    case State::Controls:
        controls_menu_.render(render_target, graphics::Render_states());
        break;
    default:
        break;
    }    

    auto& items = navigation_menu_.children();
    auto& general = *items[0];
    auto& controls = *items[1];
    auto& video = *items[2];
    auto& audio = *items[3];    
    auto& network = *items[4];
    auto& back = *items[5];

    if (controls.clicked())
    {
        switch_to(State::Controls);
    }

    if (back.clicked())
    {
        switch_to(State::None);

        back.reset_state();
        return_to_main_menu();
    }
}

void ts::states::Settings_scene::handle_event(const sf::Event& event)
{
    controls_menu_.handle_event(event);
}


void ts::states::Settings_scene::return_to_main_menu()
{
    set_visible(false);

    main_menu_->return_to_main_menu();
}

void ts::states::Settings_scene::update()
{
    if (visible())
    {
        auto mouse_state = context()->mouse_state();
        navigation_menu_.update(mouse_state);

        controls_menu_.update(mouse_state);
    }
}


ts::states::Controls_menu::Controls_menu(resources::Input_settings* input_settings)
: control_map_(convert_key_mapping(input_settings->key_mapping)),
  current_input_(controls::Control::None),
  input_settings_(input_settings)
{
    const sf::Font& font = fonts::get_font_by_type<fonts::Sans>();

    self_.create_child<gui::elements::Colored_area>(Vector2i(420, 440), sf::Color(255, 255, 255, 20));

    auto header_text = self_.create_child<gui::elements::Text_button>(Vector2i(0, 0), "Control Setup", font, 16);
    header_text->set_color(header_text_color);
    header_text->set_position(120, 15);

    auto player_slot = self_.create_child<gui::elements::Text_button>(Vector2i(0, 0), "Player slot", font, 13);
    player_slot->set_position(20, 45);

    sf::VertexArray slot_button_vertices(sf::Triangles, 3);
    slot_button_vertices[0].position = sf::Vector2f(0.0, 5.0);
    slot_button_vertices[1].position = sf::Vector2f(10.0, 0.0);
    slot_button_vertices[2].position = sf::Vector2f(10.0, 10.0);

    for (auto i = 0; i != 3; ++i) slot_button_vertices[i].color = sf::Color(255, 255, 255);

    auto slot_button_hover_vertices = slot_button_vertices;
    for (auto i = 0; i != 3; ++i) slot_button_hover_vertices[i].color = sf::Color(255, 150, 0);

    prev_slot_button_ = self_.create_child<gui::elements::Rectangular_vertex_button>(Vector2i(16, 16),
                                                                               slot_button_vertices,
                                                                               slot_button_hover_vertices);
    prev_slot_button_->set_position(220, 48);
    
    slot_button_vertices[0].position = sf::Vector2f(10.0, 5.0);
    slot_button_vertices[1].position = sf::Vector2f(0.0, 0.0);
    slot_button_vertices[2].position = sf::Vector2f(0.0, 10.0);
    for (auto i = 0; i != 3; ++i) slot_button_hover_vertices[i].position = slot_button_vertices[i].position;

    next_slot_button_ = self_.create_child<gui::elements::Rectangular_vertex_button>(Vector2i(16, 16),
                                                                               slot_button_vertices,
                                                                               slot_button_hover_vertices);
    next_slot_button_->set_position(260, 48);

    slot_id_text_ = self_.create_child<gui::elements::Text_button>(Vector2i(0, 0), "", font, 13);
    slot_id_text_->set_position(240, 45);

    Vector2i key_button_size(250, 20);
    sf::Color key_hover_color(255, 150, 0);

    auto create_key_name_text = [&](const std::string& text, Vector2i position)
    {
        auto text_item = self_.create_child<gui::elements::Text_button>(key_button_size, text, font, 13);
        text_item->set_position(position);
        text_item->set_hover_color(key_hover_color);
    };

    auto create_key_bind_text = [&](Vector2i position)
    {
        auto text_item = self_.create_child<gui::elements::Text_button>(key_button_size, "", font, 13);
        text_item->set_offset(Vector2i(200, 0));
        text_item->set_hover_color(key_hover_color);
        text_item->set_position(position);
        return text_item;
    };

    Vector2i position(20, 70);
    create_key_name_text("Accelerate", position);
    accelerate_key_ = create_key_bind_text(position);
    position.y += key_button_size.y;
    
    create_key_name_text("Brake", position);
    brake_key_ = create_key_bind_text(position);
    position.y += key_button_size.y;

    create_key_name_text("Left", position);
    left_key_ = create_key_bind_text(position);
    position.y += key_button_size.y;

    create_key_name_text("Right", position);
    right_key_ = create_key_bind_text(position);
    position.y += key_button_size.y;

    create_key_name_text("Fire", position);
    fire_key_ = create_key_bind_text(position);
    position.y += key_button_size.y;

    create_key_name_text("Alt Fire", position);
    alt_fire_key_ = create_key_bind_text(position);
    position.y += key_button_size.y;
    
    set_current_slot(0);
}

void ts::states::Controls_menu::render(graphics::Render_target& render_target, graphics::Render_states render_states)
{
    self_.render(render_target, render_states);
}

void ts::states::Controls_menu::update(const gui::mouse::State& mouse_state)
{
    using controls::Control;
    if (current_input_ == Control::None)
    {
        self_.update(mouse_state);
        
        if (next_slot_button_->clicked())
        {
            auto new_slot_id = slot_id_ + 1;
            if (new_slot_id >= 4) new_slot_id = 0;
            set_current_slot(new_slot_id);
        }

        else if (prev_slot_button_->clicked())
        {
            auto new_slot_id = slot_id_ - 1;
            if (new_slot_id < 0) new_slot_id = 3;
            set_current_slot(new_slot_id);
        }

        else if (accelerate_key_->clicked())
        {
            current_input_ = Control::Accelerate;
        }

        else if (brake_key_->clicked())
        {
            current_input_ = Control::Brake;
        }

        else if (left_key_->clicked())
        {
            current_input_ = Control::Left;
        }

        else if (right_key_->clicked())
        {
            current_input_ = Control::Right;
        }

        else if (fire_key_->clicked())
        {
            current_input_ = Control::Fire;
        }

        else if (alt_fire_key_->clicked())
        {
            current_input_ = Control::Alt_fire;
        }
    }
}

void ts::states::Controls_menu::handle_event(const sf::Event& event)
{
    if (current_input_ != controls::Control::None)
    {
        if (event.type == sf::Event::MouseButtonPressed)
        {
            current_input_ = controls::Control::None;
        }

        else if (event.type == sf::Event::KeyPressed)
        {
            auto control = current_input_;
            auto key = event.key.code;

            auto old_bind = control_map_.find(std::make_pair(slot_id_, control));
            if (old_bind != control_map_.end())
            {
                input_settings_->key_mapping.bind_key_to_control(old_bind->second, slot_id_, controls::Control::None);
            }

            input_settings_->key_mapping.bind_key_to_control(key, slot_id_, control);
            control_map_ = convert_key_mapping(input_settings_->key_mapping);

            current_input_ = controls::Control::None;
            update_text_displays();
        }
    }
}

void ts::states::Controls_menu::set_position(Vector2i position)
{
    self_.set_position(position);
}

sf::Keyboard::Key ts::states::Controls_menu::get_bound_key(controls::Slot slot_id, controls::Control control)
{
    auto bind = std::make_pair(slot_id, control);
    auto it = control_map_.find(bind);
    if (it != control_map_.end()) return it->second;

    return sf::Keyboard::Unknown;
}

void ts::states::Controls_menu::update_text_displays()
{    
    auto slot_id = slot_id_;

    slot_id_text_->set_text(std::to_string(slot_id + 1));

    using controls::Control;
    auto accelerate = get_bound_key(slot_id, Control::Accelerate);
    auto brake = get_bound_key(slot_id, Control::Brake);
    auto left = get_bound_key(slot_id, Control::Left);
    auto right = get_bound_key(slot_id, Control::Right);
    auto fire = get_bound_key(slot_id, Control::Fire);
    auto alt_fire = get_bound_key(slot_id, Control::Alt_fire);

    auto key_to_string = [](sf::Keyboard::Key key)
    {
        if (key == sf::Keyboard::Unknown) return "";

        return controls::key_to_string(key);
    };

    accelerate_key_->set_text(key_to_string(accelerate));
    brake_key_->set_text(key_to_string(brake));
    left_key_->set_text(key_to_string(left));
    right_key_->set_text(key_to_string(right));
    fire_key_->set_text(key_to_string(fire));
    alt_fire_key_->set_text(key_to_string(alt_fire));
}

void ts::states::Controls_menu::set_current_slot(controls::Slot slot_id)
{
    slot_id_ = slot_id;
    update_text_displays();
}

ts::states::Controls_menu::control_map_type ts::states::Controls_menu::convert_key_mapping(const controls::Key_mapping& key_mapping)
{
    control_map_type control_map;
    for (const auto& bind : key_mapping)
    {
        auto map_key = std::make_pair(bind.first.slot, bind.second);
        control_map[map_key] = bind.first.key;
    }

    return control_map;
}