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
#include "key_mapping.hpp"

namespace
{
    using sf::Keyboard;
    using namespace ts::controls;

    struct Bound_key
    {
        Key key;
        Slot slot;
        Control control;
    };

    Bound_key default_initializer[] = {
        { Keyboard::Up, 0, Control::Accelerate },
        { Keyboard::Down, 0, Control::Brake },
        { Keyboard::Left, 0, Control::Left },
        { Keyboard::Right, 0, Control::Right },
        { Keyboard::RControl, 0, Control::Fire },
        { Keyboard::RAlt, 0, Control::Alt_fire },

        { Keyboard::W, 1, Control::Accelerate },
        { Keyboard::S, 1, Control::Brake },
        { Keyboard::A, 1, Control::Left },
        { Keyboard::D, 1, Control::Right },
        { Keyboard::LControl, 1, Control::Fire },
        { Keyboard::LAlt, 1, Control::Alt_fire },
    };
}

ts::controls::Key_mapping ts::controls::default_key_mapping()
{
    Key_mapping key_mapping;
    for (const auto& bind : default_initializer)
    {
        key_mapping.bind_key_to_control(bind.key, bind.slot, bind.control);
    }

    return key_mapping;
}

void ts::controls::Key_mapping::unbind_control_keys(Slot slot, Control control)
{
    for (auto bind_it = key_mapping_.begin(); bind_it != key_mapping_.end();)
    {
        if (bind_it->first.slot == slot && bind_it->second == control)
        {
            bind_it = key_mapping_.erase(bind_it);
        }

        else
        {
            ++bind_it;
        }
    }
}

void ts::controls::Key_mapping::bind_key_to_control(Key key, Slot slot, Control control)
{
    unbind_control_keys(slot, control);

    Key_bind key_bind;
    key_bind.key = key;
    key_bind.slot = slot;

    if (control == Control::None) key_mapping_.erase(key_bind);
    else key_mapping_[key_bind] = control;
}

ts::controls::Control ts::controls::Key_mapping::get_control_bound_to_key(Key key, Slot slot) const
{
    Key_bind key_bind;
    key_bind.key = key;
    key_bind.slot = slot;

    auto it = key_mapping_.find(key_bind);
    if (it == key_mapping_.end()) return Control::None;

    return it->second;
}

std::pair<ts::controls::Key_mapping::Map_type::const_iterator,      
    ts::controls::Key_mapping::Map_type::const_iterator> ts::controls::Key_mapping::get_all_binds_by_key(Key key) const
{
    Key_bind dummy_bind;
    dummy_bind.key = key;

    dummy_bind.slot = std::numeric_limits<Slot>::min();
    auto begin = key_mapping_.lower_bound(dummy_bind);

    dummy_bind.slot = std::numeric_limits<Slot>::max();
    auto end = key_mapping_.upper_bound(dummy_bind);

    return std::make_pair(begin, end);
}

ts::controls::Key_mapping::Map_type::const_iterator ts::controls::Key_mapping::begin() const
{
    return key_mapping_.begin();
}

ts::controls::Key_mapping::Map_type::const_iterator ts::controls::Key_mapping::end() const
{
    return key_mapping_.end();
}


sf::Keyboard::Key ts::controls::key_from_code(std::int32_t code)
{
    switch (code)
    {
    case sf::Keyboard::A: return sf::Keyboard::A;
    case sf::Keyboard::B: return sf::Keyboard::B;
    case sf::Keyboard::C: return sf::Keyboard::C;
    case sf::Keyboard::D: return sf::Keyboard::D;
    case sf::Keyboard::E: return sf::Keyboard::E;
    case sf::Keyboard::F: return sf::Keyboard::F;
    case sf::Keyboard::G: return sf::Keyboard::G;
    case sf::Keyboard::H: return sf::Keyboard::H;
    case sf::Keyboard::I: return sf::Keyboard::I;
    case sf::Keyboard::J: return sf::Keyboard::J;
    case sf::Keyboard::K: return sf::Keyboard::K;
    case sf::Keyboard::L: return sf::Keyboard::L;
    case sf::Keyboard::M: return sf::Keyboard::M;
    case sf::Keyboard::N: return sf::Keyboard::N;
    case sf::Keyboard::O: return sf::Keyboard::O;
    case sf::Keyboard::P: return sf::Keyboard::P;
    case sf::Keyboard::Q: return sf::Keyboard::Q;
    case sf::Keyboard::R: return sf::Keyboard::R;
    case sf::Keyboard::S: return sf::Keyboard::S;
    case sf::Keyboard::T: return sf::Keyboard::T;
    case sf::Keyboard::U: return sf::Keyboard::U;
    case sf::Keyboard::V: return sf::Keyboard::V;
    case sf::Keyboard::W: return sf::Keyboard::W;
    case sf::Keyboard::X: return sf::Keyboard::X;
    case sf::Keyboard::Y: return sf::Keyboard::Y;
    case sf::Keyboard::Z: return sf::Keyboard::Z;
    case sf::Keyboard::Num0: return sf::Keyboard::Num0;
    case sf::Keyboard::Num1: return sf::Keyboard::Num1;
    case sf::Keyboard::Num2: return sf::Keyboard::Num2;
    case sf::Keyboard::Num3: return sf::Keyboard::Num3;
    case sf::Keyboard::Num4: return sf::Keyboard::Num4;
    case sf::Keyboard::Num5: return sf::Keyboard::Num5;
    case sf::Keyboard::Num6: return sf::Keyboard::Num6;
    case sf::Keyboard::Num7: return sf::Keyboard::Num7;
    case sf::Keyboard::Num8: return sf::Keyboard::Num8;
    case sf::Keyboard::Num9: return sf::Keyboard::Num9;
    case sf::Keyboard::Escape: return sf::Keyboard::Escape;
    case sf::Keyboard::LControl: return sf::Keyboard::LControl;
    case sf::Keyboard::LShift: return sf::Keyboard::LShift;
    case sf::Keyboard::LAlt: return sf::Keyboard::LAlt;
    case sf::Keyboard::LSystem: return sf::Keyboard::LSystem;
    case sf::Keyboard::RControl: return sf::Keyboard::RControl;
    case sf::Keyboard::RShift: return sf::Keyboard::RShift;
    case sf::Keyboard::RAlt: return sf::Keyboard::RAlt;
    case sf::Keyboard::RSystem: return sf::Keyboard::RSystem;
    case sf::Keyboard::Menu: return sf::Keyboard::Menu;
    case sf::Keyboard::LBracket: return sf::Keyboard::LBracket;
    case sf::Keyboard::RBracket: return sf::Keyboard::RBracket;
    case sf::Keyboard::SemiColon: return sf::Keyboard::SemiColon;
    case sf::Keyboard::Comma: return sf::Keyboard::Comma;
    case sf::Keyboard::Period: return sf::Keyboard::Period;
    case sf::Keyboard::Quote: return sf::Keyboard::Quote;
    case sf::Keyboard::Slash: return sf::Keyboard::Slash;
    case sf::Keyboard::BackSlash: return sf::Keyboard::BackSlash;
    case sf::Keyboard::Tilde: return sf::Keyboard::Tilde;
    case sf::Keyboard::Equal: return sf::Keyboard::Equal;
    case sf::Keyboard::Dash: return sf::Keyboard::Dash;
    case sf::Keyboard::Space: return sf::Keyboard::Space;
    case sf::Keyboard::Return: return sf::Keyboard::Return;
    case sf::Keyboard::BackSpace: return sf::Keyboard::BackSpace;
    case sf::Keyboard::Tab: return sf::Keyboard::Tab;
    case sf::Keyboard::PageUp: return sf::Keyboard::PageUp;
    case sf::Keyboard::PageDown: return sf::Keyboard::PageDown;
    case sf::Keyboard::End: return sf::Keyboard::End;
    case sf::Keyboard::Home: return sf::Keyboard::Home;
    case sf::Keyboard::Insert: return sf::Keyboard::Insert;
    case sf::Keyboard::Delete: return sf::Keyboard::Delete;
    case sf::Keyboard::Add: return sf::Keyboard::Add;
    case sf::Keyboard::Subtract: return sf::Keyboard::Subtract;
    case sf::Keyboard::Multiply: return sf::Keyboard::Multiply;
    case sf::Keyboard::Divide: return sf::Keyboard::Divide;
    case sf::Keyboard::Left: return sf::Keyboard::Left;
    case sf::Keyboard::Right: return sf::Keyboard::Right;
    case sf::Keyboard::Up: return sf::Keyboard::Up;
    case sf::Keyboard::Down: return sf::Keyboard::Down;
    case sf::Keyboard::Numpad0: return sf::Keyboard::Numpad0;
    case sf::Keyboard::Numpad1: return sf::Keyboard::Numpad1;
    case sf::Keyboard::Numpad2: return sf::Keyboard::Numpad2;
    case sf::Keyboard::Numpad3: return sf::Keyboard::Numpad3;
    case sf::Keyboard::Numpad4: return sf::Keyboard::Numpad4;
    case sf::Keyboard::Numpad5: return sf::Keyboard::Numpad5;
    case sf::Keyboard::Numpad6: return sf::Keyboard::Numpad6;
    case sf::Keyboard::Numpad7: return sf::Keyboard::Numpad7;
    case sf::Keyboard::Numpad8: return sf::Keyboard::Numpad8;
    case sf::Keyboard::Numpad9: return sf::Keyboard::Numpad9;
    case sf::Keyboard::F1: return sf::Keyboard::F1;
    case sf::Keyboard::F2: return sf::Keyboard::F2;
    case sf::Keyboard::F3: return sf::Keyboard::F3;
    case sf::Keyboard::F4: return sf::Keyboard::F4;
    case sf::Keyboard::F5: return sf::Keyboard::F5;
    case sf::Keyboard::F6: return sf::Keyboard::F6;
    case sf::Keyboard::F7: return sf::Keyboard::F7;
    case sf::Keyboard::F8: return sf::Keyboard::F8;
    case sf::Keyboard::F9: return sf::Keyboard::F9;
    case sf::Keyboard::F10: return sf::Keyboard::F10;
    case sf::Keyboard::F11: return sf::Keyboard::F11;
    case sf::Keyboard::F12: return sf::Keyboard::F12;
    case sf::Keyboard::F13: return sf::Keyboard::F13;
    case sf::Keyboard::F14: return sf::Keyboard::F14;
    case sf::Keyboard::F15: return sf::Keyboard::F15;
    case sf::Keyboard::Pause: return sf::Keyboard::Pause;

    default: return sf::Keyboard::Unknown;
    }
}

const char* ts::controls::key_to_string(sf::Keyboard::Key key)
{
    switch (key)
    {
    case sf::Keyboard::A: return "A";
    case sf::Keyboard::B: return "B";
    case sf::Keyboard::C: return "C";
    case sf::Keyboard::D: return "D";
    case sf::Keyboard::E: return "E";
    case sf::Keyboard::F: return "F";
    case sf::Keyboard::G: return "G";
    case sf::Keyboard::H: return "H";
    case sf::Keyboard::I: return "I";
    case sf::Keyboard::J: return "J";
    case sf::Keyboard::K: return "K";
    case sf::Keyboard::L: return "L";
    case sf::Keyboard::M: return "M";
    case sf::Keyboard::N: return "N";
    case sf::Keyboard::O: return "O";
    case sf::Keyboard::P: return "P";
    case sf::Keyboard::Q: return "Q";
    case sf::Keyboard::R: return "R";
    case sf::Keyboard::S: return "S";
    case sf::Keyboard::T: return "T";
    case sf::Keyboard::U: return "U";
    case sf::Keyboard::V: return "V";
    case sf::Keyboard::W: return "W";
    case sf::Keyboard::X: return "X";
    case sf::Keyboard::Y: return "Y";
    case sf::Keyboard::Z: return "Z";
    case sf::Keyboard::Num0: return "Num0";
    case sf::Keyboard::Num1: return "Num1";
    case sf::Keyboard::Num2: return "Num2";
    case sf::Keyboard::Num3: return "Num3";
    case sf::Keyboard::Num4: return "Num4";
    case sf::Keyboard::Num5: return "Num5";
    case sf::Keyboard::Num6: return "Num6";
    case sf::Keyboard::Num7: return "Num7";
    case sf::Keyboard::Num8: return "Num8";
    case sf::Keyboard::Num9: return "Num9";
    case sf::Keyboard::Escape: return "Escape";
    case sf::Keyboard::LControl: return "Left Control";
    case sf::Keyboard::LShift: return "Left Shift";
    case sf::Keyboard::LAlt: return "Left Alt";
    case sf::Keyboard::LSystem: return "Left System";
    case sf::Keyboard::RControl: return "Right Control";
    case sf::Keyboard::RShift: return "Right Shift";
    case sf::Keyboard::RAlt: return "Right Alt";
    case sf::Keyboard::RSystem: return "Right System";
    case sf::Keyboard::Menu: return "Menu";
    case sf::Keyboard::LBracket: return "Left Bracket";
    case sf::Keyboard::RBracket: return "Right Bracket";
    case sf::Keyboard::SemiColon: return "SemiColon";
    case sf::Keyboard::Comma: return "Comma";
    case sf::Keyboard::Period: return "Period";
    case sf::Keyboard::Quote: return "Quote";
    case sf::Keyboard::Slash: return "Slash";
    case sf::Keyboard::BackSlash: return "BackSlash";
    case sf::Keyboard::Tilde: return "Tilde";
    case sf::Keyboard::Equal: return "Equal";
    case sf::Keyboard::Dash: return "Dash";
    case sf::Keyboard::Space: return "Space";
    case sf::Keyboard::Return: return "Return";
    case sf::Keyboard::BackSpace: return "BackSpace";
    case sf::Keyboard::Tab: return "Tab";
    case sf::Keyboard::PageUp: return "Page Up";
    case sf::Keyboard::PageDown: return "Page Down";
    case sf::Keyboard::End: return "End";
    case sf::Keyboard::Home: return "Home";
    case sf::Keyboard::Insert: return "Insert";
    case sf::Keyboard::Delete: return "Delete";
    case sf::Keyboard::Add: return "Add";
    case sf::Keyboard::Subtract: return "Subtract";
    case sf::Keyboard::Multiply: return "Multiply";
    case sf::Keyboard::Divide: return "Divide";
    case sf::Keyboard::Left: return "Left";
    case sf::Keyboard::Right: return "Right";
    case sf::Keyboard::Up: return "Up";
    case sf::Keyboard::Down: return "Down";
    case sf::Keyboard::Numpad0: return "Numpad0";
    case sf::Keyboard::Numpad1: return "Numpad1";
    case sf::Keyboard::Numpad2: return "Numpad2";
    case sf::Keyboard::Numpad3: return "Numpad3";
    case sf::Keyboard::Numpad4: return "Numpad4";
    case sf::Keyboard::Numpad5: return "Numpad5";
    case sf::Keyboard::Numpad6: return "Numpad6";
    case sf::Keyboard::Numpad7: return "Numpad7";
    case sf::Keyboard::Numpad8: return "Numpad8";
    case sf::Keyboard::Numpad9: return "Numpad9";
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
    case sf::Keyboard::Pause: return "Pause";
    default: return "Unknown";
    }
}
