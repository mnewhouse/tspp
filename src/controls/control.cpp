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
#include "control.hpp"

ts::controls::Control ts::controls::control_from_string(const std::string& control_string)
{
    using controls::Control;

    if (control_string == "accelerate") return Control::Accelerate;
    if (control_string == "brake") return Control::Brake;
    if (control_string == "left") return Control::Left;
    if (control_string == "right") return Control::Right;
    if (control_string == "fire") return Control::Fire;
    if (control_string == "alt_fire") return Control::Alt_fire;

    return Control::None;
}