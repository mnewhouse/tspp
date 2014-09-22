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
#include "input_interface.hpp"

namespace ts
{
    namespace gui
    {
        unsigned int active_key_modifiers();
        mouse::Button translate_mouse_button(sf::Mouse::Button button);
    }
}

unsigned int ts::gui::active_key_modifiers()
{
    unsigned int modifiers = 0;

    using sf::Keyboard;

    if (Keyboard::isKeyPressed(Keyboard::LShift) ||
        Keyboard::isKeyPressed(Keyboard::RShift))
    {
        modifiers |= Key_modifiers::Shift;
    }

    if (Keyboard::isKeyPressed(Keyboard::LControl) ||
        Keyboard::isKeyPressed(Keyboard::RControl))
    {
        modifiers |= Key_modifiers::Ctrl;
    }

    if (Keyboard::isKeyPressed(Keyboard::LAlt) ||
        Keyboard::isKeyPressed(Keyboard::RAlt))
    {
        modifiers |= Key_modifiers::Alt;
    }

    return modifiers;
};

ts::gui::mouse::Button ts::gui::translate_mouse_button(sf::Mouse::Button button)
{
    switch (button)
    {
    case sf::Mouse::Button::Left: return mouse::Button::Left;
    case sf::Mouse::Button::Right: return mouse::Button::Right;
    case sf::Mouse::Button::Middle: return mouse::Button::Middle;
    default: return mouse::Button::None;
    }    
}

void ts::gui::SFML_Input_interface::handle_event(const sf::Event& event)
{
    switch (event.type)
    {
    case sf::Event::MouseMoved:
        process_mouse_move(Vector2i(event.mouseMove.x, event.mouseMove.y), active_key_modifiers());
        break;
        
    case sf::Event::MouseButtonPressed:
        process_mouse_button_down(translate_mouse_button(event.mouseButton.button), active_key_modifiers());
        break;

    case sf::Event::MouseButtonReleased:
        process_mouse_button_up(translate_mouse_button(event.mouseButton.button), active_key_modifiers());
        break;

    case sf::Event::MouseWheelMoved:
        process_mouse_wheel(event.mouseWheel.delta, active_key_modifiers());
        break;

    case sf::Event::KeyPressed:
        process_key_down(event.key.code, active_key_modifiers());
        break;

    case sf::Event::KeyReleased:
        process_key_up(event.key.code, active_key_modifiers());
        break;

    case sf::Event::TextEntered:
        process_text_input(event.text.unicode);

        break;

    default:
        break;
    }

}