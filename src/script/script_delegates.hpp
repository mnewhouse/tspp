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

#ifndef SCRIPT_DELEGATES_HPP
#define SCRIPT_DELEGATES_HPP

#include "delegates_impl.hpp"
#include "script_table.hpp"

namespace ts
{
    namespace script
    {
        template <typename T>
        Delegate_definition create_delegate_definition()
        {
            Delegate_definition delegates;
            delegates.type_tag = userdata_type_tag<T>();

            impl::copy_delegate_member_functions<T>(std::back_inserter(delegates.member_functions));

            return delegates;
        };
    }
}
#endif