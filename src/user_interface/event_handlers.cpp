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

#include "event_handlers.hpp"
#include <iostream>

ts::gui::mouse::State ts::gui::events::mouse_event(mouse::State state, const sf::Event& event)
{
    auto convert_button = [](sf::Mouse::Button button) -> unsigned
    {
        switch (button)
        {
        case sf::Mouse::Button::Left:
            return mouse::Button::Left;
        case sf::Mouse::Button::Right:
            return mouse::Button::Right;
        case sf::Mouse::Button::Middle:
            return mouse::Button::Middle;
        default:
            return 0;
        }      
    };

    switch (event.type)
    {
    case sf::Event::MouseMoved:
        state.position.x = event.mouseMove.x;
        state.position.y = event.mouseMove.y;
        break;

    case sf::Event::MouseButtonPressed:
        state.button_state |= convert_button(event.mouseButton.button);
        break;

    case sf::Event::MouseButtonReleased:
        state.button_state &= ~convert_button(event.mouseButton.button);
        break;

                                         
    case sf::Event::MouseWheelMoved:
        state.scroll_offset = event.mouseWheel.delta;
        break;

    default:
        break;
    }

    return state;
}