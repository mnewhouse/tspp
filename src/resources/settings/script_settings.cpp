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
#include "script_settings.hpp"

void ts::resources::Script_settings::enable_script(utf8_string script_name)
{
    auto it = std::find(loaded_scripts_.begin(), loaded_scripts_.end(), script_name);
    if (it == loaded_scripts_.end())
    {
        loaded_scripts_.push_back(std::move(script_name));
    }
}

void ts::resources::Script_settings::disable_script(const utf8_string& script_name)
{
    auto it = std::find(loaded_scripts_.begin(), loaded_scripts_.end(), script_name);
    if (it != loaded_scripts_.end())
    {
        loaded_scripts_.erase(it);
    }
}

bool ts::resources::Script_settings::is_script_enabled(const utf8_string& script_name) const
{
    return std::find(loaded_scripts_.begin(), loaded_scripts_.end(), script_name) != loaded_scripts_.end();
}

const std::vector<ts::utf8_string>& ts::resources::Script_settings::loaded_scripts() const
{
    return loaded_scripts_;
}

void ts::resources::Script_settings::set_default_gamemode(utf8_string default_gamemode)
{
    default_gamemode_ = std::move(default_gamemode);
}

const ts::utf8_string& ts::resources::Script_settings::default_gamemode() const
{
    return default_gamemode_;
}

bool ts::resources::Script_settings::force_default_gamemode() const
{
    return force_default_gamemode_;
}

void ts::resources::Script_settings::force_default_gamemode(bool force)
{
    force_default_gamemode_ = force;
}