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
#include "user_interface/elements/button.hpp"
#include "user_interface/elements/background_decorator.hpp"
#include "user_interface/elements/list.hpp"
#include "user_interface/elements/draggable.hpp"
#include "user_interface/elements/scroll_pane.hpp"
#include "user_interface/elements/grid_list.hpp"
#include "user_interface/elements/arrow.hpp"

#include "resources/track_store.hpp"
#include "resources/track.hpp"

#include "resources/car_store.hpp"
#include "resources/pattern.hpp"

#include "local_cup.hpp"

#include "core/utility.hpp"

#include <random>
#include <set>

namespace ts
{
    namespace states
    {
        class Main_menu_scene
            : public gui::Scene
        {
        public:
            Main_menu_scene(Main_menu* main_menu);            

        private:
            void show_players_menu();
            void show_settings_menu();
            void show_cup_config_menu();
            void quit();

            Main_menu* main_menu_;
        };

        class Settings_scene
            : public gui::Scene
        {
        public:
            Settings_scene(Main_menu* main_menu);

        private:
            void show_controls_menu();
            void return_to_main_menu();

            Main_menu* main_menu_;
        };

        class Player_selection_scene;

        class Players_scene
            : public gui::Scene
        {
        public:
            Players_scene(Main_menu* main_menu);

            void update_selection_list();

        private:
            void open_player_selection(controls::Slot slot_id);
            void return_to_main_menu();

            Main_menu* main_menu_;
            controls::Slot slot_id_ = 0;

            std::unique_ptr<Player_selection_scene> player_selection_scene_;
        };

        class Player_selection_scene
            : public gui::Scene
        {
        public:
            Player_selection_scene(Players_scene* players_scene);

        private:
            void return_to_players_menu();

            Players_scene* players_scene_;
        };

        class Cup_config_scene;

        class Track_selection_scene
            : public gui::Scene
        {
        public:
            Track_selection_scene(Cup_config_scene* cup_config_scene, resources::Resource_store* resource_store);

            void update_track_grid_list();

        private:
            void select_track(const resources::Track_handle& track_handle);
            void return_to_cup_menu();
            void clear_selection();
            
            void return_to_parent_directory();
            void open_sub_directory(const std::string& dir_name);
            void update_selected_tracks_header_text();
            void toggle_allow_duplicates();

            void increment_random_selection_count();
            void decrement_random_selection_count();

            void select_random_tracks();
            void center_random_selection_count();

            Cup_config_scene* cup_config_scene_;
            resources::Resource_store* resource_store_;

            gui::elements::List* selected_tracks_list_;
            gui::elements::Text_item* selected_tracks_header_;
            gui::elements::Grid_list* track_grid_list_;

            gui::elements::Button* allow_duplicates_value_;
            gui::elements::Text_item* random_selection_count_text_;
            bool grid_list_update_pending_ = true;

            resources::Track_directory track_directory_;
            
            std::mt19937 selection_randomizer_;
        };

        class Cup_config_scene
            : public gui::Scene
        {
        public:
            Cup_config_scene(Main_menu* main_menu, resources::Resource_store* resource_store);

            void update_track_selection_scene();

        private:
            void return_to_main_menu();
            void show_tracks_menu();

            Main_menu* main_menu_;

            resources::Resource_store* resource_store_;
            std::unique_ptr<Track_selection_scene> track_selection_scene_;

        };
    }
}

ts::states::Main_menu::Main_menu(state_machine_type* state_machine, gui::Context* gui_context,
                                 resources::Resource_store* resource_store)
    : gui::State(state_machine, gui_context, resource_store),
      main_menu_scene_(std::make_unique<Main_menu_scene>(this)),
      players_scene_(std::make_unique<Players_scene>(this)),
      cup_config_scene_(std::make_unique<Cup_config_scene>(this, resource_store))
{
    players_scene_->set_visible(false);
    cup_config_scene_->set_visible(false);

    add_render_scene(&*main_menu_scene_);
    add_render_scene(&*players_scene_);
    add_render_scene(&*cup_config_scene_);
}

void ts::states::Main_menu::update(std::size_t frame_duration)
{
    main_menu_scene_->update();
    players_scene_->update();
    cup_config_scene_->update();
    
    players_scene_->update_selection_list();
    cup_config_scene_->update_track_selection_scene();
}

void ts::states::Main_menu::handle_event(const sf::Event& event)
{
}

ts::states::Main_menu_scene::Main_menu_scene(Main_menu* main_menu)
: gui::Scene(main_menu->context()),
  main_menu_(main_menu)
{
    const auto& font = fonts::get_font_by_type<fonts::Sans>();

    auto screen_size = context()->screen_size();

    auto navigation_menu = root_element().create_child<gui::Element>();
    navigation_menu->set_position(20, screen_size.y - 280);

    using gui::elements::Button;
    Vector2i area_size(180, 30);
    std::uint32_t character_size = 20;

    sf::Color base_color(255, 255, 255);
    sf::Color hover_color(255, 150, 0);

    auto create_menu_item = [&](const std::string& text, Vector2i position)
    {
        auto item = navigation_menu->create_child<Button>(area_size, text, font, character_size);
        item->set_position(position);
        item->set_base_color(base_color);
        item->set_hover_color(hover_color);
        item->set_offset(20, 0);
        item->set_size(area_size);
        return item;
    };

    Vector2i position(0, 0);

    auto item = create_menu_item("Players", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) { show_players_menu(); });

    position.y += area_size.y;
    item = create_menu_item("Local Game", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) { show_cup_config_menu(); });

    position.y += area_size.y;
    item = create_menu_item("Network Game", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) {  });

    position.y += area_size.y;
    item = create_menu_item("Editor", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) {  });

    position.y += area_size.y;
    item = create_menu_item("Settings", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) { show_settings_menu(); });

    position.y += area_size.y;
    item = create_menu_item("Quit", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) { quit(); });   

}

void ts::states::Main_menu_scene::show_settings_menu()
{
}

void ts::states::Main_menu_scene::show_players_menu()
{
    set_visible(false);

    main_menu_->show_players_menu();
}

void ts::states::Main_menu_scene::show_cup_config_menu()
{
    set_visible(false);

    main_menu_->show_cup_config_menu();
}

ts::states::Settings_scene::Settings_scene(Main_menu* main_menu)
: gui::Scene(main_menu->context()),
  main_menu_(main_menu)
{
    const auto& font = fonts::get_font_by_type<fonts::Sans>();

    auto screen_size = context()->screen_size();    

    auto navigation_menu = root_element().create_child<gui::Element>();
    navigation_menu->set_position(20, screen_size.y - 280);

    using gui::elements::Button;
    Vector2i area_size(180, 30);
    std::uint32_t character_size = 20;

    sf::Color base_color(255, 255, 255);
    sf::Color hover_color(255, 150, 0);

    auto create_menu_item = [&](const std::string& text, Vector2i position)
    {
        auto item = navigation_menu->create_child<Button>(area_size, text, font, character_size);
        item->set_position(position);
        item->set_base_color(base_color);
        item->set_hover_color(hover_color);
        item->set_offset(20, 0);
        item->set_size(area_size);
        return item;
    };

    Vector2i position(0, 0);
    auto item = create_menu_item("General", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) {  });

    position.y += area_size.y;
    item = create_menu_item("Controls", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) {  });

    position.y += area_size.y;
    item = create_menu_item("Video", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) {  });

    position.y += area_size.y;
    item = create_menu_item("Audio", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) {  });

    position.y += area_size.y;
    item = create_menu_item("Network", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) {  });

    position.y += area_size.y;
    item = create_menu_item("Back", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) {  });

    
}

void ts::states::Settings_scene::return_to_main_menu()
{
    set_visible(false);

    main_menu_->return_to_main_menu();
}

ts::states::Players_scene::Players_scene(Main_menu* main_menu)
: gui::Scene(main_menu->context()),
  main_menu_(main_menu),
  player_selection_scene_(std::make_unique<Player_selection_scene>(this))
{
    const auto& font = fonts::get_font_by_type<fonts::Sans>();

    using gui::elements::Button;

    auto container = root_element().create_child<gui::Element>();
    container->set_size(Vector2i(400, 440));
    container->set_position(200, context()->screen_size().y - 460);
    container->create_decorator<gui::elements::Background_color>(sf::Color(255, 255, 255, 15));


    auto header = container->create_child<gui::elements::Text_item>("Player Setup", font, 14);
    header->set_position(140, 10);
    header->set_color(sf::Color(180, 180, 180));

    Vector2i area_size(300, 30);
    std::uint32_t character_size = 14;

    sf::Color base_color(255, 255, 255);
    sf::Color hover_color(255, 150, 0);

    auto back = container->create_child<Button>(Vector2i(85, 26), "Back", font, 18);
    back->set_base_color(base_color);
    back->set_hover_color(hover_color);
    back->set_position(10, 400);
    back->set_offset(20, 0);
    back->add_event_handler(gui::Event_onLeftClick(), 
                            [this](const gui::Element& element, Vector2i position)
    {
        return_to_main_menu();
    });

    auto create_player_slot_button = [&](Vector2i position, controls::Slot slot)
    {
        auto callback = [this, slot](const gui::Element& element, Vector2i position)
        {
            open_player_selection(slot);
        };

        auto item = container->create_child<Button>(area_size, "Player " + std::to_string(slot + 1), font, character_size);
        item->set_base_color(base_color);
        item->set_hover_color(hover_color);
        item->add_event_handler(gui::Event_onLeftClick(), callback);
        item->set_offset(20, 0);
        item->set_position(position);
    };

    Vector2i position(10, 45);
    for (controls::Slot slot = 0; slot != 4; ++slot)
    {
        create_player_slot_button(position, slot);
        position.y += area_size.y;
    }

    player_selection_scene_->set_visible(false);
    main_menu_->add_render_scene(&*player_selection_scene_);
}

void ts::states::Players_scene::update_selection_list()
{
    player_selection_scene_->update();
}

void ts::states::Players_scene::return_to_main_menu()
{
    set_visible(false);

    main_menu_->return_to_main_menu();
}

void ts::states::Players_scene::open_player_selection(controls::Slot slot_id)
{
    slot_id_ = slot_id;

    set_visible(false);

    player_selection_scene_->set_visible(true);
}

ts::states::Player_selection_scene::Player_selection_scene(Players_scene* player_scene)
: gui::Scene(player_scene->context()),
  players_scene_(player_scene)
{
    auto container = root_element().create_child<gui::Element>();
    container->set_size(Vector2i(400, 440));
    container->set_position(200, context()->screen_size().y - 460);
    container->create_decorator<gui::elements::Background_color>(sf::Color(255, 255, 255, 15));

    const auto& font = fonts::get_font_by_type<fonts::Sans>();

    auto header = container->create_child<gui::elements::Text_item>("Select Player", font, 14);
    header->set_position(140, 10);
    header->set_color(sf::Color(180, 180, 180));

    using gui::elements::Button;
    
    sf::Color base_color(255, 255, 255);
    sf::Color hover_color(255, 150, 0);    

    auto back = container->create_child<Button>(Vector2i(85, 26), "Back", font, 18);
    back->set_base_color(base_color);
    back->set_hover_color(hover_color);
    back->set_position(10, 400);
    back->set_offset(20, 0);
    back->add_event_handler(gui::Event_onLeftClick(), 
                            [this](const gui::Element& element, Vector2i position)
    {
        return_to_players_menu();
    });

    auto scroll_pane = container->create_child<gui::elements::Scroll_pane>(Vector2i(360, 200));
    scroll_pane->set_position(20, 30);



    auto list = scroll_pane->create_child<gui::elements::List>(Vector2i(320, 20), sf::Color(255, 255, 255, 0), 
                                                               sf::Color(255, 255, 255, 15));

    

    list->add_row<gui::elements::Button>("Candy", font, 14);
    list->add_row<gui::elements::Button>("Cheese", font, 14);
    list->add_row<gui::elements::Button>("Bacon", font, 14);
    list->add_row<gui::elements::Button>("Chicken", font, 14);
    list->add_row<gui::elements::Button>("Sandwich", font, 14);
    list->add_row<gui::elements::Button>("Steak", font, 14);
    list->add_row<gui::elements::Button>("Broccoli", font, 14);
    list->add_row<gui::elements::Button>("Carrot", font, 14);
    list->add_row<gui::elements::Button>("Pepper", font, 14);
    list->add_row<gui::elements::Button>("Habanero", font, 14);
    list->add_row<gui::elements::Button>("Jalapeno", font, 14);
    list->add_row<gui::elements::Button>("Chili", font, 14);
    list->add_row<gui::elements::Button>("Mushroom", font, 14);
    list->add_row<gui::elements::Button>("Pork", font, 14);
    list->add_row<gui::elements::Button>("Cabbage", font, 14);
    list->add_row<gui::elements::Button>("Soy", font, 14);
}

void ts::states::Player_selection_scene::return_to_players_menu()
{
    set_visible(false);

    players_scene_->set_visible(true);
}


ts::states::Cup_config_scene::Cup_config_scene(Main_menu* main_menu, resources::Resource_store* resource_store)
: gui::Scene(main_menu->context()),
  main_menu_(main_menu),
  resource_store_(resource_store),
  track_selection_scene_(std::make_unique<Track_selection_scene>(this, resource_store))
{
    const auto& font = fonts::get_font_by_type<fonts::Sans>();

    auto screen_size = context()->screen_size();

    auto navigation_menu = root_element().create_child<gui::Element>();
    navigation_menu->set_position(20, screen_size.y - 280);

    using gui::elements::Button;
    Vector2i area_size(180, 30);
    std::uint32_t character_size = 20;

    sf::Color base_color(255, 255, 255);
    sf::Color hover_color(255, 150, 0);

    auto create_menu_item = [&](const std::string& text, Vector2i position)
    {
        auto item = navigation_menu->create_child<Button>(area_size, text, font, character_size);
        item->set_position(position);
        item->set_base_color(base_color);
        item->set_hover_color(hover_color);
        item->set_offset(20, 0);
        item->set_size(area_size);
        return item;
    };

    Vector2i position(0, 22);

    auto header = navigation_menu->create_child<gui::elements::Text_item>("Cup Setup", font, 14);
    header->set_position(30, 0);
    header->set_color(sf::Color(180, 180, 180));
    

    auto item = create_menu_item("Tracks", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) { show_tracks_menu(); });

    position.y += area_size.y;
    item = create_menu_item("Cars", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) {  });

    position.y += area_size.y;
    item = create_menu_item("Resources", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) {  });

    position.y += area_size.y;
    item = create_menu_item("Back", position);
    item->add_event_handler(gui::Event_onLeftClick(),
                            [this](const gui::Element& element, Vector2i position) { return_to_main_menu(); });

    main_menu->add_render_scene(&*track_selection_scene_);
    track_selection_scene_->set_visible(false);

}

void ts::states::Cup_config_scene::return_to_main_menu()
{
    set_visible(false);
    track_selection_scene_->set_visible(false);

    main_menu_->return_to_main_menu();    
}

void ts::states::Cup_config_scene::show_tracks_menu()
{
    track_selection_scene_->set_visible(true);
}

void ts::states::Cup_config_scene::update_track_selection_scene()
{
    track_selection_scene_->update_track_grid_list();

    track_selection_scene_->update();
}

ts::states::Track_selection_scene::Track_selection_scene(Cup_config_scene* cup_config_scene, resources::Resource_store* resource_store)
: gui::Scene(cup_config_scene->context()),
cup_config_scene_(cup_config_scene),
resource_store_(resource_store),
track_directory_(resource_store->tracks.root_directory()),
selection_randomizer_(std::random_device{}())
{
    auto screen_size = context()->screen_size();

    auto container_size = screen_size;
    container_size.x -= 240;
    container_size.y -= 40;

    if (container_size.x >= 780) container_size.x = 780;
    if (container_size.y >= 600) container_size.y = 600;

    auto container = root_element().create_child<gui::Element>();
    container->set_size(container_size);
    container->set_position(200, screen_size.y - container_size.y - 20);
    container->create_decorator<gui::elements::Background_color>(sf::Color(255, 255, 255, 15));

    const auto& font = fonts::get_font_by_type<fonts::Sans>();

    auto header = container->create_child<gui::elements::Text_item>("Track Selection", font, 16);
    header->set_color(sf::Color(180, 180, 180));
    header->set_position((container_size.x >> 1) - 50, 10);

    auto scroll_pane = container->create_child<gui::elements::Scroll_pane>(Vector2i(container_size.x - 60, 400));
    scroll_pane->set_position(20, 40);   

    track_grid_list_ = scroll_pane->create_child<gui::elements::Grid_list>(Vector2i(130, 20), (container_size.x - 60) / 130);

    selected_tracks_header_ = container->create_child<gui::elements::Text_item>("", font, 14);
    selected_tracks_header_->set_color(sf::Color(180, 180, 180));
    selected_tracks_header_->set_position(60, 300);

    auto selected_tracks_scroll_pane = container->create_child<gui::elements::Scroll_pane>
        (Vector2i(240, (container_size.y - 60) / 2));

    selected_tracks_list_ = selected_tracks_scroll_pane->create_child<gui::elements::List>
        (Vector2i(240, 20), sf::Color(255, 255, 255, 0), sf::Color(255, 255, 255, 10));

    selected_tracks_scroll_pane->set_position(20, 320);

    selected_tracks_scroll_pane->create_decorator<gui::elements::Background_color>(sf::Color(255, 255, 255, 10));

    update_selected_tracks_header_text();

    Vector2i area_size(140, 24);

    auto clear = container->create_child<gui::elements::Button>(area_size, "Clear selection", font, 16);
    auto allow_duplicates = container->create_child<gui::elements::Button>(Vector2i(250, 24), "Allow duplicates", font, 16);

    allow_duplicates_value_ = container->create_child<gui::elements::Button>(Vector2i(250, 24), "Yes", font, 16);
    

    auto select_random = container->create_child<gui::elements::Button>(area_size, "Select random", font, 16);

    

    sf::Color base_color(255, 255, 255);
    sf::Color hover_color(255, 150, 0);

    auto random_left_arrow = container->create_child<gui::elements::Left_arrow>(10, base_color, hover_color);
    auto random_right_arrow = container->create_child<gui::elements::Right_arrow>(10, base_color, hover_color);

    auto random_select_count = resource_store->settings.track_settings.random_count;


    auto move_up_arrow = container->create_child<gui::elements::Up_arrow>(14, base_color, hover_color);
    auto move_down_arrow = container->create_child<gui::elements::Down_arrow>(14, base_color, hover_color);

    auto remove_selection = container->create_child<gui::elements::Button>(Vector2i(20, 20), "x", font, 20);
    
    random_selection_count_text_ = container->create_child<gui::elements::Text_item>(std::to_string(random_select_count), font, 16);
    random_selection_count_text_->set_size(40, 30);
    random_selection_count_text_->center_horizontal();


    clear->set_position(300, 320);
    allow_duplicates->set_position(300, 345);
    allow_duplicates_value_->set_position(300, 345);
    select_random->set_position(300, 370);

    random_left_arrow->set_position(480, 375);
    random_right_arrow->set_position(530, 375);
    random_selection_count_text_->set_position(490, 370);

    allow_duplicates_value_->set_offset(200, 0);

    clear->set_base_color(base_color);
    allow_duplicates->set_base_color(base_color);
    select_random->set_base_color(base_color);
    allow_duplicates_value_->set_base_color(base_color);

    clear->set_hover_color(hover_color);
    allow_duplicates->set_hover_color(hover_color);
    select_random->set_hover_color(hover_color);
    allow_duplicates_value_->set_hover_color(hover_color);

    move_up_arrow->set_position(270, container_size.y - 90);
    move_down_arrow->set_position(270, container_size.y - 70);

    remove_selection->set_base_color(sf::Color(255, 255, 255));
    remove_selection->set_hover_color(sf::Color(255, 0, 0));
    remove_selection->set_position(262, container_size.y - 50);
    remove_selection->set_size(30, 30);
    remove_selection->center_horizontal();

    clear->add_event_handler(gui::Event_onLeftClick(),
                             [this](const gui::Element& element, Vector2i position) { clear_selection(); });

    allow_duplicates->add_event_handler(gui::Event_onLeftClick(),
                                        [this](const gui::Element& element, Vector2i position) { toggle_allow_duplicates(); });

    random_left_arrow->add_event_handler(gui::Event_onLeftClick(),
                                         [this](const gui::Element& element, Vector2i position) { decrement_random_selection_count(); });

    random_right_arrow->add_event_handler(gui::Event_onLeftClick(),
                                         [this](const gui::Element& element, Vector2i position) { increment_random_selection_count(); });

    select_random->add_event_handler(gui::Event_onLeftClick(),
                                     [this](const gui::Element& element, Vector2i position) { select_random_tracks(); });
}

void ts::states::Track_selection_scene::update_track_grid_list()
{
    if (!grid_list_update_pending_) return;

    const auto& font = fonts::get_font_by_type<fonts::Sans>();

    track_grid_list_->clear();

    if (track_directory_)
    {
        auto parent_directory = track_grid_list_->create_field<gui::elements::Button>("../", font, 14);
        parent_directory->set_base_color(sf::Color(150, 150, 150));
        parent_directory->set_hover_color(sf::Color(255, 150, 0));
        parent_directory->set_offset(10, 0);

        auto parent_directory_event_handler = [this](const gui::Element& element, Vector2i position)
        {
            return_to_parent_directory();
        };

        parent_directory->add_event_handler(gui::Event_onLeftClick(), parent_directory_event_handler);

        for (auto it = track_directory_.dir_begin(); it != track_directory_.dir_end(); ++it)
        {
            auto dir = *it;
            auto dir_name = dir.dir_name();

            auto button = track_grid_list_->create_field<gui::elements::Button>(dir_name + '/', font, 14);
            button->set_base_color(sf::Color(150, 150, 150));
            button->set_hover_color(sf::Color(255, 150, 0));
            button->set_offset(10, 0);

            auto sub_directory_event_handler = [this, dir_name](const gui::Element& element, Vector2i position)
            {
                open_sub_directory(dir_name);
            };

            button->add_event_handler(gui::Event_onLeftClick(), sub_directory_event_handler);
        }

        for (auto it = track_directory_.track_begin(); it != track_directory_.track_end(); ++it)
        {
            auto track_handle = *it;
            auto button = track_grid_list_->create_field<gui::elements::Button>(track_handle.name(), font, 14);
            button->set_base_color(sf::Color(255, 255, 255));
            button->set_hover_color(sf::Color(255, 150, 0));
            button->set_offset(10, 0);

            auto event_handler = [this, track_handle](const gui::Element& element, Vector2i position)
            {
                select_track(track_handle);
            };

            button->add_event_handler(gui::Event_onLeftClick(), event_handler);
        }
    }
    
    grid_list_update_pending_ = false;
}

void ts::states::Track_selection_scene::open_sub_directory(const std::string& dir_name)
{
    auto sub_directory = track_directory_.sub_directory(dir_name);
    if (sub_directory)
    {
        track_directory_ = sub_directory;
        grid_list_update_pending_ = true;
    }
}

void ts::states::Track_selection_scene::update_selected_tracks_header_text()
{
    auto& selected_tracks = resource_store_->settings.track_settings.selected_tracks;
    selected_tracks_header_->set_text("Selected tracks (" + std::to_string(selected_tracks.size()) + ")");
}

void ts::states::Track_selection_scene::return_to_parent_directory()
{
    auto parent_directory = track_directory_.parent_directory();
    if (parent_directory)
    {
        track_directory_ = parent_directory;
        grid_list_update_pending_ = true;
    }
}

void ts::states::Track_selection_scene::select_track(const resources::Track_handle& track_handle)
{
    auto& selected_tracks = resource_store_->settings.track_settings.selected_tracks;
    selected_tracks.push_back(track_handle.path());

    const auto& font = fonts::get_font_by_type<fonts::Sans>();

    selected_tracks_list_->add_row<gui::elements::Text_item>(track_handle.name(), font, 14);

    update_selected_tracks_header_text();
}

void ts::states::Track_selection_scene::select_random_tracks()
{
    auto duplicates_allowed = resource_store_->settings.track_settings.allow_duplicates;
    auto count = resource_store_->settings.track_settings.random_count;
    auto& selected_tracks = resource_store_->settings.track_settings.selected_tracks;

    auto prior_size = selected_tracks.size();

    std::map<std::string, std::string> random_choices;
    for (auto it = track_directory_.track_begin(); it != track_directory_.track_end(); ++it)
    {
        auto track_handle = *it;
        random_choices[track_handle.path()] = track_handle.name();
    }

    auto random_track = [&]() -> std::pair<std::string, std::string>
    {
        auto size = random_choices.size();

        std::uniform_int_distribution<std::uint32_t> dist(0, size - 1);

        auto random_id = dist(selection_randomizer_);
        return *std::next(random_choices.begin(), random_id);
    };

    const auto& font = fonts::get_font_by_type<fonts::Sans>();

    if (!duplicates_allowed)
    {
        for (auto& track : selected_tracks)
        {
            random_choices.erase(track);
        }
    }

    for (std::uint32_t n = 0; n != count && !random_choices.empty(); ++n)
    {
        auto track = random_track();

        if (!duplicates_allowed)
        {
            random_choices.erase(track.first);
        }        

        selected_tracks_list_->add_row<gui::elements::Text_item>(track.second, font, 14);

        selected_tracks.push_back(std::move(track.first));
    }

    update_selected_tracks_header_text();
}

void ts::states::Track_selection_scene::clear_selection()
{
    selected_tracks_list_->clear();

    auto& selected_tracks = resource_store_->settings.track_settings.selected_tracks;
    selected_tracks.clear();

    grid_list_update_pending_ = true;
    update_selected_tracks_header_text();
}

void ts::states::Track_selection_scene::toggle_allow_duplicates()
{
    auto& allowed = resource_store_->settings.track_settings.allow_duplicates;
    allowed = !allowed;

    allow_duplicates_value_->set_text(allowed ? "Yes" : "No");
}

void ts::states::Track_selection_scene::increment_random_selection_count()
{
    auto& random_count = resource_store_->settings.track_settings.random_count;
    ++random_count;

    random_selection_count_text_->set_text(std::to_string(random_count));
}

void ts::states::Track_selection_scene::decrement_random_selection_count()
{
    auto& random_count = resource_store_->settings.track_settings.random_count;
    if (random_count > 1)
    {
        --random_count;
        random_selection_count_text_->set_text(std::to_string(random_count));
    }
}

void ts::states::Main_menu_scene::quit()
{
    main_menu_->quit();
}

void ts::states::Main_menu::return_to_main_menu()
{
    main_menu_scene_->set_visible(true);
}

void ts::states::Main_menu::show_players_menu()
{
    players_scene_->set_visible(true);
}

void ts::states::Main_menu::show_cup_config_menu()
{
    cup_config_scene_->set_visible(true);
}

void ts::states::Main_menu::quit()
{
    state_machine()->clear();
}

