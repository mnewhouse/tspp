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
#include "client_script_engine.hpp"

#include "script/script_engine.hpp"
#include "script/script_module.hpp"

ts::client::Script_engine::Script_engine()
    : engine_(std::make_unique<script::Engine>())
{
}

ts::client::Script_engine::~Script_engine()
{
}

const ts::script::Engine* ts::client::Script_engine::engine() const
{
    return engine_.get();
}

ts::script::Engine* ts::client::Script_engine::engine()
{
    return engine_.get();
}

void ts::client::Script_engine::trigger_event(const utf8_string& event, const script::Value& source,
                                              const std::vector<script::Value>& arguments) const
{
    engine_->trigger_event(event, source, arguments);
}