/*
 * Turbo Sliders++
 * Copyright (C) 2013 Martin Newhouse
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

#include "key_mapping.hpp"

namespace
{
    using ts::controls::Control;
    using sf::Keyboard;

    ts::controls::Key_mapping::value_type default_initializer[] = {
        { Keyboard::Up, { 1, Control::Accelerate } },
        { Keyboard::Down, { 1, Control::Brake } },
        { Keyboard::Left, { 1, Control::Left } },
        { Keyboard::Right, { 1, Control::Right } },
        { Keyboard::RControl, { 1, Control::Fire } },
        { Keyboard::RAlt, { 1, Control::Alt_fire } },

        { Keyboard::W, { 2, Control::Accelerate } },
        { Keyboard::S, { 2, Control::Brake } },
        { Keyboard::A, { 2, Control::Left } },
        { Keyboard::D, { 2, Control::Right } },
        { Keyboard::LControl, { 2, Control::Fire } },
        { Keyboard::LAlt, { 2, Control::Alt_fire } },
    };
}

ts::controls::Key_mapping ts::controls::default_key_mapping()
{
    return Key_mapping(std::begin(default_initializer), std::end(default_initializer));
}

