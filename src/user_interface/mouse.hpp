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

#ifndef GUI_MOUSE_HPP
#define GUI_MOUSE_HPP

namespace ts
{
    namespace gui
    {
        namespace mouse
        {
            enum class Button
            {
                None = 0,
                Left = 1,
                Right = 2,
                Middle = 3
            };

            bool operator==(Button button, unsigned int button_id);
            bool operator==(unsigned int button_id, Button button);

            bool operator!=(Button button, unsigned int button_id);
            bool operator!=(unsigned int button_id, Button button);
        }
    }
}

inline bool ts::gui::mouse::operator==(Button button, unsigned int button_id)
{
    return static_cast<unsigned int>(button) == button_id;
}

inline bool ts::gui::mouse::operator==(unsigned int button_id, Button button)
{
    return button == button_id;
}

inline bool ts::gui::mouse::operator!=(Button button, unsigned int button_id)
{
    return !(button == button_id);
}

inline bool ts::gui::mouse::operator!=(unsigned int button_id, Button button)
{
    return !(button == button_id);
}

#endif