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
#include "player_color.hpp"


ts::resources::Player_color::Player_color(sf::Color color)
{
    base_color.type = Color_base::Plain;
    base_color.primary_color = color;
}

ts::utf8_string ts::resources::to_string(Color_base::Type type)
{
    using Type = Color_base::Type;

    switch (type)
    {
    case Type::Plain: return "Plain";
    case Type::Vertical_gradient: return "Vertical gradient";
    case Type::Horizontal_gradient: return "Horizontal gradient";
    default: return "";
    }
}

ts::utf8_string ts::resources::to_string(Color_extra::Type type)
{
    using Type = Color_extra::Type;

    switch (type)
    {
    case Type::Vertical_stripe: return "Vertical stripe";
    case Type::Gradient_stripe: return "Gradient stripe";
    case Type::Double_stripe: return "Double stripe";
    case Type::Horizontal_stripe: return "Horizontal stripe";
    case Type::Reflected_gradient: return "Reflected gradient";
    case Type::Front_stripe: return "Front stripe";
    case Type::Rear_stripe: return "Rear stripe";
    case Type::Front_half: return "Front half";
    case Type::Rear_half: return "Rear half";
    case Type::Left_half: return "Left half";
    case Type::Right_half: return "Right half";
    default: return "";
    }
}