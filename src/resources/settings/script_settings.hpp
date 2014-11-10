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

#ifndef SCRIPT_SETTINGS_HPP
#define SCRIPT_SETTINGS_HPP

namespace ts
{
    namespace resources
    {
        class Script_settings
        {
        public:
            void enable_script(utf8_string script_name);
            void disable_script(const utf8_string& script_name);
            bool is_script_enabled(const utf8_string& script_name) const;

            const std::vector<utf8_string>& loaded_scripts() const;

        private:
            std::vector<utf8_string> loaded_scripts_;
        };
    }
}

#endif