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

#pragma once

#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include "graphics/colored_text.hpp"

namespace ts
{
    namespace cup    
    {
        using Chat_message = graphics::Colored_text;
        using Composite_message = graphics::Composite_text;
        using Message_color = sf::Color;

        Composite_message format_chat_message(const utf8_string& sender, const utf8_string& message);
    }
}
#endif