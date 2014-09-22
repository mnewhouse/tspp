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

#ifndef GUI_BACKGROUND_STYLE_HPP
#define GUI_BACKGROUND_STYLE_HPP

#include "background.hpp"

namespace ts
{
    namespace gui
    {
        template <typename BackgroundType>
        struct Concrete_background_style
        {
            std::shared_ptr<BackgroundType> background;
        };

        struct Background_style
        {
            Background_style() = default;

            template <typename BackgroundType>
            Background_style(Concrete_background_style<BackgroundType> concrete_style);

            std::shared_ptr<Background_base> background;
        };

        template <typename BackgroundType, typename... Args>
        Concrete_background_style<BackgroundType> make_background_style(Args&&... args);
    }
}

template <typename BackgroundType>
ts::gui::Background_style::Background_style(Concrete_background_style<BackgroundType> concrete_style)
: background(concrete_style.background)
{
}

template <typename BackgroundType, typename... Args>
ts::gui::Concrete_background_style<BackgroundType> ts::gui::make_background_style(Args&&... args)
{
    Concrete_background_style<BackgroundType> style;
    style.background = std::make_shared<BackgroundType>(std::forward<Args>(args)...);
    return style;
}

#endif
