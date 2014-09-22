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
#include "settings_menu.hpp"
#include "main_menu.hpp"

#include "gui_definitions/window_template.hpp"

#include "user_interface/elements/elements.hpp"

namespace ts
{
    namespace states
    {
        gui::Element* create_generic_settings_window(gui::Context* context, gui::Element* parent, utf8_string window_title);
        utf8_string to_string(sf::Keyboard::Key key);
    }
}

ts::states::Settings_menu::Settings_menu(Main_menu* main_menu)
: main_menu_(main_menu),
  settings_(&main_menu->resource_store()->settings),

  settings_navigation_(create_navigation(main_menu->context())),
  control_settings_(std::make_unique<Control_settings_menu>(main_menu->context(), &settings_->input_settings)),
  video_settings_(std::make_unique<Video_settings_menu>(main_menu->context(), &settings_->video_settings))
{
}

ts::gui::Element* ts::states::create_generic_settings_window(gui::Context* context, gui::Element* parent, utf8_string window_title)
{
    auto parent_size = parent->size();
    Vector2i window_size(360, 400);

    auto y_position = parent_size.y - 40.0 - window_size.y;

    auto window = gui_definitions::create_styled_window(context, parent, window_size);
    window->set_position({ 240.0, y_position });

    gui::Text_style text_style;
    text_style.font = context->font_library().font_by_name(gui::fonts::Sans);
    text_style.color = sf::Color::White;
    text_style.character_size = 16;

    auto header_text = window->create_child<gui::Text_element>(std::move(window_title), text_style);
    header_text->set_position({ 0.0, 30.0 });
    header_text->center_horizontal();

    return window;
}

ts::gui::Document_handle ts::states::Settings_menu::create_navigation(gui::Context* context)
{
    auto document = context->create_document("settings-navigation");

    auto screen_size = context->screen_size();

    const auto& font_library = context->font_library();

    gui::Text_style text_style;
    text_style.font = font_library.font_by_name(gui::fonts::Sans);
    text_style.color = sf::Color::White;
    text_style.character_size = 16;

    gui::Text_style hover_style = text_style;
    hover_style.color = sf::Color(255, 150, 0);

    gui::Vertical_list_style list_style;
    list_style.row_size = { 200.0, 24.0 };

    using navigation_list = gui::Vertical_list<gui::Text_element>;

    auto container = gui_definitions::create_styled_window(context, document.get(), Vector2i(200, 200));
    container->set_position({ 40.0, screen_size.y - 280.0 });

    auto nav = container->create_child<navigation_list>(list_style);
    nav->set_position({ 40.0, 30.0 });

    gui::Text_element::Styler_type text_styler(text_style);
    text_styler.register_style(gui::states::hover, hover_style);

    auto keys = nav->create_row("Keys", text_styler);
    auto video = nav->create_row("Video", text_styler);
    auto audio = nav->create_row("Audio", text_styler);
    auto network = nav->create_row("Network", text_styler);
    auto back = nav->create_row("Back", text_styler);
    
    keys->add_event_handler(gui::events::on_click,
                            [this](const gui::Element&)
    {
        show_key_setup();
    });

    video->add_event_handler(gui::events::on_click,
                             [this](const gui::Element&)
    {
        show_video_settings();
    });

    back->add_event_handler(gui::events::on_click, 
                            [this](const gui::Element&)
    {
        return_to_main_menu();
    });

    return document;
}


void ts::states::Settings_menu::show()
{
    settings_navigation_->set_visible(true);

    control_settings_->show();
}

void ts::states::Settings_menu::hide()
{
    settings_navigation_->set_visible(false);

    hide_all_submenus();
}

void ts::states::Settings_menu::hide_all_submenus()
{
    control_settings_->hide();
    video_settings_->hide();
}

void ts::states::Settings_menu::show_key_setup()
{
    hide_all_submenus();

    control_settings_->show();
}

void ts::states::Settings_menu::show_video_settings()
{
    hide_all_submenus();

    video_settings_->show();
}

void ts::states::Settings_menu::return_to_main_menu()
{
    hide();

    main_menu_->show();
}

ts::states::Control_settings_menu::Control_settings_menu(gui::Context* context, resources::Input_settings* input_settings)
: context_(context),
  input_settings_(input_settings)
{
    create_document(context);
}

void ts::states::Control_settings_menu::show()
{
    document_->set_visible(true);

    input_settings_.load();
    set_current_slot(0);
}

void ts::states::Control_settings_menu::hide()
{
    document_->set_visible(false);

    input_settings_.save();
}

void ts::states::Control_settings_menu::load_key_binds(const resources::Input_settings& input_settings, controls::Slot slot)
{
    slot_selection_->set_value(slot + 1);

    const char none[] = "<none>";

    key_bind_text_.accelerate->set_text(none);
    key_bind_text_.brake->set_text(none);
    key_bind_text_.left->set_text(none);
    key_bind_text_.right->set_text(none);
    key_bind_text_.fire->set_text(none);

    for (const auto& mapping : input_settings.key_mapping)
    {
        auto key_bind = mapping.first;
        auto control = mapping.second;
        if (key_bind.slot != slot) continue;

        using controls::Control;

        switch (control)
        {
        case Control::Accelerate:
            key_bind_text_.accelerate->set_text(to_string(key_bind.key));
            break;

        case Control::Brake:
            key_bind_text_.brake->set_text(to_string(key_bind.key));
            break;

        case Control::Left:
            key_bind_text_.left->set_text(to_string(key_bind.key));
            break;

        case Control::Right:
            key_bind_text_.right->set_text(to_string(key_bind.key));
            break;

        case Control::Fire:
            key_bind_text_.fire->set_text(to_string(key_bind.key));
            break;
        }
    }
}

void ts::states::Control_settings_menu::create_document(gui::Context* context)
{
    document_ = context->create_document("control-settings");

    auto window = create_generic_settings_window(context, document_.get(), "Key Setup");
    auto window_size = window->size();

    const auto& font_library = context->font_library();

    gui::Text_style text_style;
    text_style.font = font_library.font_by_name(gui::fonts::Sans);
    text_style.color = sf::Color::White;
    text_style.character_size = 14;

    auto hover_style = text_style;
    hover_style.color = sf::Color(255, 150, 0);
    
    using settings_list_type = gui::Vertical_list<gui::Element>;
    gui::Vertical_list_style list_style;
    list_style.row_size = Vector2i(300, 24);
    list_style.item_style = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 10));

    auto settings_list = window->create_child<settings_list_type>(list_style);
    settings_list->set_position({ 40.0, 60.0 });

    gui::Integer_input_style slot_input_style;
    slot_input_style.text_style = text_style;
    slot_input_style.text_hover_style = hover_style;
    slot_input_style.arrow_area_size = Vector2i(20, 24);
    slot_input_style.text_area_size = Vector2i(20, 24);

    gui::Integer_input_range slot_range;
    slot_range.min = 1;
    slot_range.max = 4;
    slot_range.value = 0;

    auto slot_input_row = settings_list->create_row();
    auto player_slot_text = slot_input_row->create_child<gui::Text_element>("Player slot:", text_style);    

    slot_selection_ = slot_input_row->create_child<gui::Integer_input>(slot_input_style, slot_range);
    slot_selection_->set_position({ 190.0, 0.0 });

    Vector2<double> key_bind_text_size(120.0, 24.0);
    Vector2<double> key_bind_text_position(160.0, 0.0);

    gui::Text_element::Styler_type key_text_styler(text_style);
    key_text_styler.register_style(gui::states::hover, hover_style);
    key_text_styler.register_style(gui::states::selected, hover_style);

    auto create_key_bind_row = [=](utf8_string setting)
    {
        auto row = settings_list->create_row();
        auto setting_text = row->create_child<gui::Text_element>(std::move(setting), text_style);

        auto text = row->create_child<gui::Text_element>("", key_text_styler);
        text->set_position(key_bind_text_position);
        text->set_size(key_bind_text_size);
        text->center_text_horizontal();
        return text;
    };

    key_bind_text_.accelerate = create_key_bind_row("Accelerate:");
    key_bind_text_.brake = create_key_bind_row("Brake:");
    key_bind_text_.left = create_key_bind_row("Left:");
    key_bind_text_.right = create_key_bind_row("Right:");
    key_bind_text_.fire = create_key_bind_row("Fire:");

    slot_selection_->add_event_handler(gui::Integer_input::on_value_change, 
                                       [this](const gui::Integer_input&, controls::Slot new_value)
    {
        set_current_slot(new_value - 1);
    });

    auto add_key_bind_handler = [this](gui::Text_element* key_bind_text, controls::Control associated_control)
    {
        key_bind_text->add_event_handler(gui::events::on_click, 
                                         [this, associated_control, key_bind_text](const gui::Element&)
        {
            start_control_assignment(key_bind_text, associated_control);
        });
    };

    document_->add_event_handler(gui::events::on_key_down, 
                                 [this](const gui::Element&, sf::Keyboard::Key key, unsigned int modifiers)
    {
        assign_key_to_pending_control(key);
    });

    document_->add_event_handler(gui::events::on_click, 
                                 [this](const gui::Element&)
    {
        cancel_control_assignment();
    });

    using controls::Control;
    add_key_bind_handler(key_bind_text_.accelerate, Control::Accelerate);
    add_key_bind_handler(key_bind_text_.brake, Control::Brake);
    add_key_bind_handler(key_bind_text_.left, Control::Left);
    add_key_bind_handler(key_bind_text_.right, Control::Right);
    add_key_bind_handler(key_bind_text_.fire, Control::Fire);
}

void ts::states::Control_settings_menu::start_control_assignment(gui::Text_element* key_text, controls::Control control)
{
    input_control_ = control;
    input_key_text_ = key_text;

    key_text->set_state(gui::states::selected, true);
}

void ts::states::Control_settings_menu::cancel_control_assignment()
{
    input_control_ = controls::Control::None;

    if (input_key_text_ != nullptr)
    {
        input_key_text_->set_state(gui::states::selected, false);
        input_key_text_ = nullptr;
    }
}

void ts::states::Control_settings_menu::assign_key_to_pending_control(sf::Keyboard::Key key)
{
    if (input_key_text_ != nullptr && input_control_ != controls::Control::None)
    {
        if (key == sf::Keyboard::Escape)
        {
            input_settings_->key_mapping.unbind_control_keys(current_slot_, input_control_);
        }

        else
        {
            input_settings_->key_mapping.bind_key_to_control(key, current_slot_, input_control_);
        }

        load_key_binds(*input_settings_, current_slot_);

        input_key_text_->set_state(gui::states::selected, false);
    }

    input_key_text_ = nullptr;
    input_control_ = controls::Control::None;
}

void ts::states::Control_settings_menu::set_current_slot(controls::Slot new_slot)
{
    if (new_slot != current_slot_)
    {
        load_key_binds(*input_settings_, new_slot);

        current_slot_ = new_slot;
    }
}

ts::utf8_string ts::states::to_string(sf::Keyboard::Key key)
{
    switch (key)
    {
        case sf::Keyboard::A: return "a";
        case sf::Keyboard::B: return "b";
        case sf::Keyboard::C: return "c";
        case sf::Keyboard::D: return "d";
        case sf::Keyboard::E: return "e";
        case sf::Keyboard::F: return "f";
        case sf::Keyboard::G: return "g";
        case sf::Keyboard::H: return "h";
        case sf::Keyboard::I: return "i";
        case sf::Keyboard::J: return "j";
        case sf::Keyboard::K: return "k";
        case sf::Keyboard::L: return "l";
        case sf::Keyboard::M: return "m";
        case sf::Keyboard::N: return "n";
        case sf::Keyboard::O: return "o";
        case sf::Keyboard::P: return "p";
        case sf::Keyboard::Q: return "q";
        case sf::Keyboard::R: return "r";
        case sf::Keyboard::S: return "s";
        case sf::Keyboard::T: return "t";
        case sf::Keyboard::U: return "u";
        case sf::Keyboard::V: return "v";
        case sf::Keyboard::W: return "w";
        case sf::Keyboard::X: return "x";
        case sf::Keyboard::Y: return "y";
        case sf::Keyboard::Z: return "z";
        case sf::Keyboard::Num0: return "0";
        case sf::Keyboard::Num1: return "1";
        case sf::Keyboard::Num2: return "2";
        case sf::Keyboard::Num3: return "3";
        case sf::Keyboard::Num4: return "4";
        case sf::Keyboard::Num5: return "5";
        case sf::Keyboard::Num6: return "6";
        case sf::Keyboard::Num7: return "7";
        case sf::Keyboard::Num8: return "8";
        case sf::Keyboard::Num9: return "9";
        case sf::Keyboard::Escape: return "escape";
        case sf::Keyboard::LControl: return "left ctrl";
        case sf::Keyboard::LShift: return "left shift";
        case sf::Keyboard::LAlt: return "left alt";
        case sf::Keyboard::LSystem: return "left system";
        case sf::Keyboard::RControl: return "left ctrl";
        case sf::Keyboard::RShift: return "right shift";
        case sf::Keyboard::RAlt: return "right alt";
        case sf::Keyboard::RSystem: return "right system";
        case sf::Keyboard::Menu: return "menu";
        case sf::Keyboard::LBracket: return "[";
        case sf::Keyboard::RBracket: return "]";
        case sf::Keyboard::SemiColon: return ";";
        case sf::Keyboard::Comma: return ",";
        case sf::Keyboard::Period: return ".";
        case sf::Keyboard::Quote: return "'";
        case sf::Keyboard::Slash: return "/";
        case sf::Keyboard::BackSlash: return "\\";
        case sf::Keyboard::Tilde: return "~";
        case sf::Keyboard::Equal: return "=";
        case sf::Keyboard::Dash: return "-";
        case sf::Keyboard::Space: return "space";
        case sf::Keyboard::Return: return "return";
        case sf::Keyboard::BackSpace: return "backspace";
        case sf::Keyboard::Tab: return "tab";
        case sf::Keyboard::PageUp: return "page up";
        case sf::Keyboard::PageDown: return "page down";
        case sf::Keyboard::End: return "end";
        case sf::Keyboard::Home: return "home";
        case sf::Keyboard::Insert: return "insert";
        case sf::Keyboard::Delete: return "delete";
        case sf::Keyboard::Add: return "[+]";
        case sf::Keyboard::Subtract: return "[-]";
        case sf::Keyboard::Multiply: return "[*]";
        case sf::Keyboard::Divide: return "[/]";
        case sf::Keyboard::Left: return "left";
        case sf::Keyboard::Right: return "right";
        case sf::Keyboard::Up: return "up";
        case sf::Keyboard::Down: return "down";
        case sf::Keyboard::Numpad0: return "[0]";
        case sf::Keyboard::Numpad1: return "[1]";
        case sf::Keyboard::Numpad2: return "[2]";
        case sf::Keyboard::Numpad3: return "[3]";
        case sf::Keyboard::Numpad4: return "[4]";
        case sf::Keyboard::Numpad5: return "[5]";
        case sf::Keyboard::Numpad6: return "[6]";
        case sf::Keyboard::Numpad7: return "[7]";
        case sf::Keyboard::Numpad8: return "[8]";
        case sf::Keyboard::Numpad9: return "[9]";
        case sf::Keyboard::F1: return "F1";
        case sf::Keyboard::F2: return "F2";
        case sf::Keyboard::F3: return "F3";
        case sf::Keyboard::F4: return "F4";
        case sf::Keyboard::F5: return "F5";
        case sf::Keyboard::F6: return "F6";
        case sf::Keyboard::F7: return "F7";
        case sf::Keyboard::F8: return "F8";
        case sf::Keyboard::F9: return "F9";
        case sf::Keyboard::F10: return "F10";
        case sf::Keyboard::F11: return "F11";
        case sf::Keyboard::F12: return "F12";
        case sf::Keyboard::F13: return "F13";
        case sf::Keyboard::F14: return "F14";
        case sf::Keyboard::F15: return "F15";
        case sf::Keyboard::Pause: return "pause";
        default: return "";
    }
}

ts::states::Video_settings_menu::Video_settings_menu(gui::Context* context, resources::Video_settings* video_settings)
: context_(context),
  video_settings_(video_settings)
{
    create_document(context);
}

void ts::states::Video_settings_menu::show()
{
    document_->set_visible(true);
}

void ts::states::Video_settings_menu::hide()
{
    document_->set_visible(false);
}

void ts::states::Video_settings_menu::create_document(gui::Context* context)
{
    document_ = context->create_document("video-settings");

    auto window = create_generic_settings_window(context, document_.get(), "Video Settings");

    
    auto window_size = window->size();

    const auto& font_library = context->font_library();

    gui::Text_style text_style;
    text_style.font = font_library.font_by_name(gui::fonts::Sans);
    text_style.color = sf::Color::White;
    text_style.character_size = 14;

    auto hover_style = text_style;
    hover_style.color = sf::Color(255, 150, 0);
    
    using settings_list_type = gui::Vertical_list<gui::Element>;
    gui::Vertical_list_style list_style;
    list_style.row_size = Vector2i(300, 24);
    list_style.item_style = gui::make_background_style<gui::Plain_background>(sf::Color(255, 255, 255, 10));

    auto settings_list = window->create_child<settings_list_type>(list_style);
    settings_list->set_position({ 40.0, 60.0 });

    auto create_option_row = [=](utf8_string setting_string)
    {
        auto row = settings_list->create_row();
        row->create_child<gui::Text_element>(std::move(setting_string), text_style);

        return row;
    };

    gui::Option_set_style option_set_style;
    option_set_style.arrow_area_size = Vector2i(20, 24);
    option_set_style.text_area_size = Vector2i(90, 24);
    option_set_style.text_style = text_style;
    option_set_style.text_hover_style = hover_style;

    auto resolution_row = create_option_row("Resolution:");
    auto screen_modes = sf::VideoMode::getFullscreenModes();
    resolution_option_set_ = resolution_row->create_child<resolution_option_set_type>(option_set_style);
    resolution_option_set_->set_position({ 160.0, 0.0 });

    auto current_resolution = video_settings_->screen_resolution;
    sf::VideoMode current_video_mode(current_resolution.x, current_resolution.y);
    if (current_video_mode.isValid())
    {
        screen_modes.push_back(current_video_mode);
    }
    
    for (const auto& screen_mode : screen_modes)
    {
        auto resolution = std::make_pair(screen_mode.width, screen_mode.height);
        utf8_string resolution_string = std::to_string(resolution.first);
        resolution_string += "x";
        resolution_string += std::to_string(resolution.second);

        resolution_option_set_->add_option(resolution, std::move(resolution_string));
    }

    auto full_screen_row = create_option_row("Full screen:");
    full_screen_option_set_ = full_screen_row->create_child<gui::Option_set<bool>>(option_set_style);
    full_screen_option_set_->set_position({ 160.0, 0.0 });
    full_screen_option_set_->add_option(true, "On");
    full_screen_option_set_->add_option(false, "Off");

    auto vsync_row = create_option_row("Vertical sync:");
    vsync_option_set_ = vsync_row->create_child<gui::Option_set<bool>>(option_set_style);
    vsync_option_set_->set_position({ 160.0, 0.0 });
    vsync_option_set_->add_option(true, "On");
    vsync_option_set_->add_option(false, "Off");

    resolution_option_set_->set_value(std::make_pair(current_resolution.x, current_resolution.y));
    full_screen_option_set_->set_value(video_settings_->full_screen);
    vsync_option_set_->set_value(video_settings_->vertical_sync);

    resolution_option_set_->add_event_handler(resolution_option_set_type::on_change,
                                              [this](const gui::Element& element, std::pair<std::int32_t, std::int32_t> new_value)
    {
        video_settings_->screen_resolution.x = new_value.first;
        video_settings_->screen_resolution.y = new_value.second;
    });

    full_screen_option_set_->add_event_handler(gui::Option_set<bool>::on_change,
                                               [this](const gui::Element& element, bool full_screen)
    {
        video_settings_->full_screen = full_screen;
    });

    vsync_option_set_->add_event_handler(gui::Option_set<bool>::on_change,
                                         [this](const gui::Element& element, bool vsync_enabled)
    {
        video_settings_->vertical_sync = vsync_enabled;
    });

}