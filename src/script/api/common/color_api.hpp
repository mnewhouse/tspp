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

#ifndef COLOR_API_HPP
#define COLOR_API_HPP

#include "script/script_api.hpp"

#include <SFML/Graphics/Color.hpp>

namespace ts
{
    namespace script_api
    {
        namespace classes
        {
            static const char Color[] = "Color";
        }

        namespace members
        {
            namespace color
            {
                static const char r[] = "r";
                static const char g[] = "g";
                static const char b[] = "b";
                static const char a[] = "a";
            };
        }

        script::API_definition color_api();

        struct Color_reader
        {
        public:
            Color_reader(sf::Color& result);

            bool operator()(HSQUIRRELVM vm, SQInteger index) const;

        private:
            sf::Color& result_;
        };
    }
}

#endif